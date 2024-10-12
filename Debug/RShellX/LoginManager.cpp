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

LoginManager::LoginManager()
{
    mIsLogin = false;
    mCurPid = getpid();
    mShellPid = -1;
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

int LoginManager::SetStdioCloexec()
{
    // Set FD_CLOEXEC flag on standard file descriptors (0, 1, 2) to prevent
    // inheritance by child processes.
    // for (int fd = 0; fd <= 2; fd++) {
    //     int flags = fcntl(fd, F_GETFD);
    //     if (flags == -1) {
    //         continue;
    //     }
    //     fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
    // }
    return 0;
}

int LoginManager::ListenPipeEvent(int pipeFd)
{
    mPipePtr = std::make_shared<PPipe>(pipeFd, [&](int fd, std::string buf, void *arg) {
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
    EpollEventHandler::GetInstance()->AddPoll(mPipePtr.get());
    return 0;
}

int LoginManager::Init()
{
    SetStdioCloexec();
    ListenPipeEvent(mOutPipe[0]);

    // fork a child process to run shell
    mShellPid = fork();
    SPR_LOGD("fork shellPid = %d\n", mShellPid);
    if (mShellPid == 0) {
        ShellEnv shellEnv(mInPipe[0], mOutPipe[1], mOutPipe[1]);
        shellEnv.Init();
    }

    return 0;
}

int LoginManager::BuildConnectAsTcpServer(short port)
{
    auto pEpoll = EpollEventHandler::GetInstance();
    mTcpSrvPtr = make_shared<PSocket>(AF_INET, SOCK_STREAM, 0, [&](int cli, void *arg) {
        PSocket* pSrvObj = (PSocket*)arg;
        if (pSrvObj == nullptr) {
            SPR_LOGE("PSocket is nullptr\n");
            return;
        }

        auto tcpClient = make_shared<PSocket>(cli, [&](int sock, void *arg) {
            PSocket* pCliObj = (PSocket*)arg;
            if (pCliObj == nullptr) {
                SPR_LOGE("PSocket is nullptr\n");
                return;
            }

            // Mimics the input display format of a remote terminal session.
            // Example:
            // # pwd
            // /tmp/Release/Bin
            std::string rBuf;
            int rc = pCliObj->Read(sock, rBuf);
            if (rc <= 0) {
                mTcpClients.remove_if([sock, pEpoll, pCliObj](shared_ptr<PSocket>& v) {
                    pEpoll->DelPoll(pCliObj);
                    return (v->GetEpollFd() == sock);
                });
                return;
            }

            SPR_LOGD("# RECV [%d]> %s\n", sock, rBuf.c_str());
            std::string tmpCmd = rBuf;
            tmpCmd.erase(std::find_if(tmpCmd.rbegin(), tmpCmd.rend(), [](unsigned char ch) {
                return ch != '\r' && ch != '\n';
            }).base(), tmpCmd.end());

            if (tmpCmd == "exit" || tmpCmd == "Ctrl C" || tmpCmd == "Ctrl Q") {
                if (mShellPid == -1) {
                    SPR_LOGD("shell not running");
                    return;
                }

                SPR_LOGD("# EXIT shell %d\n", mShellPid);
                kill(mShellPid, SIGKILL);
                wait(nullptr);
                mShellPid = -1;
            } else if (tmpCmd == "exit all") {
                SPR_LOGD("# EXIT all shell\n");
                kill(mShellPid, SIGKILL);
                wait(nullptr);
                exit(EXIT_SUCCESS);
            } else {
                SPR_LOGD("# PIPE [%d]> %s\n", mInPipe[1], tmpCmd.c_str());
                int wsize = write(mInPipe[1], rBuf.c_str(), rBuf.size());
                if (wsize == -1) {
                    SPR_LOGE("write error: %s\n", strerror(errno));
                }
            }
        });

        tcpClient->AsTcpClient();
        pEpoll->AddPoll(tcpClient.get());
        mTcpClients.push_back(tcpClient);
    });

    mTcpSrvPtr->AsTcpServer(port, 5);
    pEpoll->AddPoll(mTcpSrvPtr.get());
    return 0;
}

int LoginManager::ConnectLoop()
{
    EpollEventHandler::GetInstance()->EpollLoop(true);
    return 0;
}
