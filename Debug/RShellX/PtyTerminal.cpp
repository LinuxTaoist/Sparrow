/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PtyTerminal.cpp
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
#include <memory>
#include <pty.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "SprLog.h"
#include "PPipe.h"
#include "PSocket.h"
#include "PtyTerminal.h"
#include "EpollEventHandler.h"

#define LOG_TAG "PtyTerminal"

using namespace std;

static void SigChldHandler(int signo)
{
    (void)signo;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

PtyTerminal::PtyTerminal(const std::function<void(int32_t ret, std::string, void*)>& cb, void* arg)
    : mArg(arg), mMasterFd(-1), mSlaveFd(-1), mCb(cb)
{
}

PtyTerminal::~PtyTerminal()
{
    if (mMasterFd != -1) {
        SPR_LOGI("# CLOSE MASTER FD: %d", mMasterFd);
        close(mMasterFd);
        mMasterFd = -1;
    }
    if (mSlaveFd != -1) {
        SPR_LOGI("# CLOSE SLAVE FD: %d", mSlaveFd);
        close(mSlaveFd);
        mSlaveFd = -1;
    }
}

int32_t PtyTerminal::Write(const std::string& bytes)
{
    return write(mMasterFd, bytes.c_str(), bytes.size());
}

int32_t PtyTerminal::Init()
{
    int32_t ret = openpty(&mMasterFd, &mSlaveFd, nullptr, nullptr, nullptr);
    if (ret != 0) {
        SPR_LOGE("openpty failed! (%s)", strerror(errno));
        return ret;
    }

    int32_t pid = fork();
    if (pid == 0) {             // Child process
        BashProcess();
    } else if (pid > 0) {       // Parent process
        MasterProcess();
    } else {                    // fork failed
        SPR_LOGE("fork failed! (%s)", strerror(errno));
    }

    return 0;
}

int32_t PtyTerminal::BashProcess()
{
    // Set the slave fd as the controlling terminal
    setsid();
    ioctl(mSlaveFd, TIOCSCTTY, 0);

    // Redirect stdin, stdout, and stderr
    dup2(mSlaveFd, STDIN_FILENO);
    dup2(mSlaveFd, STDOUT_FILENO);
    dup2(mSlaveFd, STDERR_FILENO);

    // close all other file descriptors
    for (int fd = sysconf(_SC_OPEN_MAX); fd > 2; fd--) {
        if (fcntl(fd, F_GETFD) != -1) {
            close(fd);
        }
    }

    // Set the terminal attributes
    struct termios term;
    int32_t rc = tcgetattr(STDIN_FILENO, &term);
    if (rc == 0) {
        term.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ICANON | ISIG | IEXTEN);
        term.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
        term.c_oflag &= ~OPOST;
        term.c_cflag &= ~(CSIZE | PARENB);
        term.c_cflag |= CS8;
        tcsetattr(STDIN_FILENO, TCSANOW, &term);
    }

    // No Color Output
    setenv("TERM", "dumb", 1);
    unsetenv("LS_COLORS");
    unsetenv("GREP_COLORS");
    unsetenv("COLORTERM");

    std::string shell = GetCurShell();  // /bin/sh
    execl(shell.c_str(), shell.c_str(), "-l", nullptr);
    SPR_LOGE("execl %s failed! (%s)", shell.c_str(), strerror(errno));
    exit(EXIT_FAILURE);
    return 0;
}

int32_t PtyTerminal::MasterProcess()
{
    close(mSlaveFd);
    signal(SIGCHLD, SigChldHandler);

    mPtyPipe = std::make_shared<PPipe>(mMasterFd, [&](ssize_t ret, std::string bytes, void* arg){
        PPipe* pPipe = reinterpret_cast<PPipe*>(arg);
        if (pPipe == nullptr) {
            SPR_LOGE("pPipe is nullptr!\n");
            return;
        }

        // When erase color, the output is messed such as vi
        // std::string out;
        // EraseColor(bytes, out);
        if (mCb) {
            mCb(ret, bytes, mArg ? mArg : this);
        }
    });

    mPtyPipe->AddToPoll();
    return 0;
}

int32_t PtyTerminal::EraseColor(const std::string& in, std::string& out)
{
    out.clear();
    out.reserve(in.size());

    const size_t len = in.size();
    size_t i = 0;

    while (i < len) {
        if (in[i] == '\033') {
            i++;
            if (i < len && in[i] == '[') {
                i++;
                while (i < len && !isalpha(static_cast<unsigned char>(in[i]))) {
                    i++;
                }
                if (i < len) {
                    i++;
                }
            } else if (i < len) {
                i++;
            }
        } else {
            out.push_back(in[i]);
            i++;
        }
    }

    return (int32_t)(out.size());
}

std::string PtyTerminal::GetCurShell()
{
    const char* shell = getenv("SHELL");
    return shell ? shell : "/bin/sh";
}
