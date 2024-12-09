/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : LoginManager.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/10/13
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/10/13 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <list>
#include <string>
#include <memory>
#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include "SprLog.h"
#include "ShellEnv.h"
#include "EpollEventHandler.h"
#include "LoginManager.h"

using namespace std;

#define SPR_LOGD(fmt, args...) LOGD("RShellLoginM", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("RShellLoginM", fmt, ##args)

pid_t LoginManager::mShellPid = -1;

LoginManager::LoginManager()
{
    mIsLogin = false;
    mCurPid = getpid();
    mStdin = dup(STDIN_FILENO);
    mStdout = dup(STDOUT_FILENO);
    mStderr = dup(STDERR_FILENO);
    int rc1 = pipe(mInPipe);
    if (rc1 != 0) {
        SPR_LOGE("pipe error: %s", strerror(errno));
    }
    int rc2 = pipe(mOutPipe);
    if (rc2 != 0) {
        SPR_LOGE("pipe error: %s", strerror(errno));
    }
}

LoginManager::~LoginManager()
{
    if (mShellPid > 0) {
        kill(mShellPid, SIGKILL);
        mShellPid = -1;
    }
    dup2(mStdin, STDIN_FILENO);
    dup2(mStdout, STDOUT_FILENO);
    dup2(mStderr, STDERR_FILENO);
    close(mInPipe[0]);
    close(mInPipe[1]);
    close(mOutPipe[0]);
    close(mOutPipe[1]);
}

LoginManager* LoginManager::GetInstance()
{
    static LoginManager instance;
    return &instance;
}

int LoginManager::ListenPipeEvent(int pipeFd)
{
    mpPipe = std::make_shared<PPipe>(pipeFd, [&](int fd, std::string buf, void *arg) {
        if (buf.empty()) {
            buf = "No return.";
        }

        buf += "\r\n# ";
        SPR_LOGD("# PIPE RECV [%d]< %s\n", fd, buf.c_str());
        // int rc = write(stdPipes[1], buf.c_str(), buf.size());
        // if (rc > 0) {
        //     // SPR_LOGD("# SEND [%d]> %s\n", sock, out.c_str());
        // } else {
        //     SPR_LOGE("# SEND failed!\n");
        // }
    });

    mpPipe->AddToPoll();
    return 0;
}

int LoginManager::RegisterSignal()
{
    signal(SIGCHLD, [](int) {
        pid_t pid;
        int status;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            mShellPid = -1;
            SPR_LOGD("Shell [%d] exit!\n", pid);
        }
    });
    return 0;
}

int LoginManager::Init()
{
    RegisterSignal();
    ListenPipeEvent(mOutPipe[0]);
    return 0;
}

int LoginManager::Usage()
{
    WriteStdin("-----------------  RShellX  -----------------\n");
    WriteStdin("Usage:\n");
    WriteStdin("Quit    : exit current shell fork\n");
    WriteStdin("Quit all: exit RShellX\n");
    WriteStdin("Help    : show help\n");
    WriteStdin("---------------------------------------------\n");
    return 0;
}

int LoginManager::ExitShell()
{
    if (mShellPid == -1 || mShellPid == 0) {
        SPR_LOGD("Shell not running!\n");
        return 0;
    }

    SPR_LOGD("Exit shell %d\n", mShellPid);
    kill(mShellPid, SIGKILL);
    mShellPid = -1;
    return 0;
}

int LoginManager::ExitAll()
{
    SPR_LOGD("Exit all\n");
    if (mShellPid > 0) {
        kill(mShellPid, SIGKILL);
    }
    exit(EXIT_SUCCESS);
    return 0;
}

int LoginManager::WriteStdin(const std::string& buf)
{
    int rc = write(STDOUT_FILENO, buf.c_str(), buf.size());
    if (rc < 0) {
        SPR_LOGE("Write stdin failed! %s", strerror(errno));
    }
    return 0;
}

int LoginManager::ExecuteCmd(string& cmdBytes)
{
    cmdBytes.erase(std::find_if(cmdBytes.rbegin(), cmdBytes.rend(), [](unsigned char ch) {
        return ch != '\r' && ch != '\n';
    }).base(), cmdBytes.end());

    if (cmdBytes == "Quit" || cmdBytes == "Ctrl C" || cmdBytes == "Ctrl Q") {
        return ExitShell();
    } else if (cmdBytes == "Quit all") {
        return ExitAll();
    } else if (cmdBytes == "Help" || cmdBytes == "help" || cmdBytes == "?") {
        return Usage();
    }

    // 上一个命令未执行完，输入作为参数传入上一个命令
    if (mShellPid > 0) {
        SPR_LOGD("Last shell %d is running, send [%s] as parameter\n", mShellPid, cmdBytes.c_str());
        // int rc = write(mInPipe[1], cmdBytes.c_str(), cmdBytes.size());
        // if (rc > 0) {
        //     SPR_LOGD("# SEND [%d]> %s\n", mInPipe[1], cmdBytes.c_str());
        // }
        return 0;
    }

    ShellEnv shellEnv(mInPipe[0], mOutPipe[1], mOutPipe[1]);
    mShellPid = shellEnv.Execute(cmdBytes);
    return 0;
}

int LoginManager::BuildConnectAsTcpServer(short port)
{
    EpollEventHandler* pEpoll = EpollEventHandler::GetInstance();

    mpTcpSrv = make_shared<PTcpServer>([&](int cli, void *arg) {
        PTcpServer* pSrvObj = (PTcpServer*)arg;
        if (pSrvObj == nullptr) {
            SPR_LOGE("pSrvObj is nullptr\n");
            return;
        }

        auto tcpClient = make_shared<PTcpClient>(cli, [&](int sock, void *arg) {
            PTcpClient* pCliObj = (PTcpClient*)arg;
            if (pCliObj == nullptr) {
                SPR_LOGE("pCliObj is nullptr\n");
                return;
            }

            std::string rBuf;
            int rc = pCliObj->Read(sock, rBuf);
            if (rc <= 0) {
                mTcpClients.remove_if([sock, pEpoll, pCliObj](shared_ptr<PTcpClient>& v) {
                    pCliObj->Close();
                    return (v->GetEvtFd() == sock);
                });
                return;
            }

            // SPR_LOGD("# RECV [%d]> %s shellpid = %d\n", sock, rBuf.c_str(), mShellPid);
            ExecuteCmd(rBuf);
        });

        tcpClient->AsTcpClient();
        mTcpClients.push_back(tcpClient);
        dup2(mpTcpSrv->GetEvtFd(), STDIN_FILENO);
        dup2(tcpClient->GetEvtFd(), STDOUT_FILENO);
        dup2(tcpClient->GetEvtFd(), STDERR_FILENO);

        const string welcomes = "Welcome to RShellX! >_<\n";
        if (write(STDOUT_FILENO, welcomes.c_str(), welcomes.size()) < 0) {
            SPR_LOGE("# Write welcome failed! %s", strerror(errno));
        }
    });

    mpTcpSrv->AsTcpServer(port, 5);
    return 0;
}

int LoginManager::ConnectLoop()
{
    EpollEventHandler::GetInstance()->EpollLoop();
    return 0;
}
