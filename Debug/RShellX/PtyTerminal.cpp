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
#include <pty.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "SprLog.h"
#include "PtyTerminal.h"

#define LOG_TAG "PtyTerminal"

static void SigChldHandler(int signo)
{
    (void)signo;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

PtyTerminal::PtyTerminal()
    : mClientFd(-1), mMasterFd(-1), mSlaveFd(-1)
{
}

PtyTerminal::~PtyTerminal()
{
    if (mMasterFd != -1) {
        close(mMasterFd);
        mMasterFd = -1;
    }
    if (mSlaveFd != -1) {
        close(mSlaveFd);
        mSlaveFd = -1;
    }
    if (mClientFd != -1) {
        close(mClientFd);
        mClientFd = -1;
    }
}

int32_t PtyTerminal::Init(int32_t clientFd)
{
    int32_t ret = openpty(&mMasterFd, &mSlaveFd, nullptr, nullptr, nullptr);
    if (ret != 0) {
        SPR_LOGE("openpty failed! (%s)", strerror(errno));
        return ret;
    }

    int32_t cmdPid = fork();
    if (cmdPid == -1) {
        SPR_LOGE("fork failed! (%s)", strerror(errno));
        return -1;
    }

    // Execute shell in child process
    if (cmdPid == 0) {
        close(mMasterFd);
        close(clientFd);

        // Set the slave fd as the controlling terminal
        setsid();
        ioctl(mSlaveFd, TIOCSCTTY, 0);

        dup2(mSlaveFd, STDIN_FILENO);
        dup2(mSlaveFd, STDOUT_FILENO);
        dup2(mSlaveFd, STDERR_FILENO);
        close(mSlaveFd);

        // Set the terminal attributes
        struct termios term;
        if (tcgetattr(STDIN_FILENO, &term) == 0) {
            term.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
            term.c_lflag |= (ICANON | ISIG | IEXTEN);
            tcsetattr(STDIN_FILENO, TCSANOW, &term);
        }

        // No Color Output
        setenv("TERM", "dumb", 1);
        execl("/bin/bash", "bash", -1, nullptr);
        SPR_LOGE("execl failed! (%s)", strerror(errno));
        exit(EXIT_FAILURE);
    }

    close(mSlaveFd);
    signal(SIGCHLD, SigChldHandler);

    SetNonBlock(mMasterFd);
    SetNonBlock(mClientFd);

    fd_set readfds;
    const int32_t MAX_BUFF_SIZE = 1024;
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(mMasterFd, &readfds);
        FD_SET(mClientFd, &readfds);

        int32_t maxFd = (mMasterFd > mClientFd) ? mMasterFd : mClientFd;
        int32_t ret = select(maxFd + 1, &readfds, nullptr, nullptr, nullptr);
        if (ret < 0 && errno != EINTR) {
            SPR_LOGE("select failed! (%s)", strerror(errno));
            break;
        }

        if (FD_ISSET(mMasterFd, &readfds)) {
            char buf[MAX_BUFF_SIZE] = {0};
            int32_t n = read(mMasterFd, buf, MAX_BUFF_SIZE);
            if (n <= 0) {
                break;
            }

            char filedBuffer[MAX_BUFF_SIZE] = {0};
            ssize_t filtedLen = FilterColorCode(buf, n, filedBuffer);
            if (filtedLen > 0) {
                write(mClientFd, filedBuffer, filtedLen);
            }
        }

        if (FD_ISSET(mClientFd, &readfds)) {
            char buf[MAX_BUFF_SIZE] = {0};
            int32_t n = read(mClientFd, buf, MAX_BUFF_SIZE);
            if (n <= 0) {
                break;
            }
            write(mMasterFd, buf, n);
        }
    }

    // clear resources
    kill(cmdPid, SIGTERM);
    close(mMasterFd);
    close(mClientFd);
    mMasterFd = -1;
    mClientFd = -1;
    return 0;
}

void PtyTerminal::SetNonBlock(int32_t fd)
{
    int32_t flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

ssize_t PtyTerminal::FilterColorCode(char* input, ssize_t len, char* output)
{
    ssize_t outputLen = 0;
    int32_t inEscape = 0;
    for (ssize_t i = 0; i < len; ++i) {
        if (input[i] == '\033') {
            inEscape = 1;
        } else if (inEscape) {
            if ((input[i] >= 'A' && input[i] <= 'Z') ||
                (input[i] >= 'a' && input[i] <= 'z') ||
                input[i] == '[' || input [i] == '(' || input[i] == ')' ||
                input[i] == '*' || input[i] == '?' || input[i] == '!' ||
                input[i] == '#' || input[i] == ' ' || input[i] == '`' ||
                input[i] == ')' || input[i] == '~') {
                inEscape = 0;
            }
        } else {
            output[outputLen++] = input[i];
        }
    }

    return outputLen;
}
