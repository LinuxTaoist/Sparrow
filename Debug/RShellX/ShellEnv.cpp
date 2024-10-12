/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ShellEnv.cpp
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
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "SprLog.h"
#include "ShellEnv.h"

#define SPR_LOGD(fmt, args...) LOGD("RShellEnv", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("RShellEnv", fmt, ##args)

ShellEnv::ShellEnv(int inFd, int outFd, int errFd)
{
    mInFd = inFd;
    mOutFd = outFd;
    mErrFd = errFd;
    mStdin = dup(STDIN_FILENO);
    mStdout = dup(STDOUT_FILENO);
    mStderr = dup(STDERR_FILENO);
    dup2(inFd, STDIN_FILENO);
    dup2(outFd, STDOUT_FILENO);
    dup2(errFd, STDERR_FILENO);
    // close(mInFd);
    // close(mOutFd);
    // close(mErrFd);
}

ShellEnv::~ShellEnv()
{
    dup2(mStdin, STDIN_FILENO);
    dup2(mStdout, STDOUT_FILENO);
    dup2(mStderr, STDERR_FILENO);
    close(mInFd);
    close(mOutFd);
    close(mErrFd);
    SPR_LOGD("Exit shell environment");
}

void ShellEnv::Init()
{
    SPR_LOGD("Enter shell environment, pid = %d std = %d %d %d", getpid(), mInFd, mOutFd, mErrFd);
    execlp("bash", "bash", "-i", nullptr);
    exit(EXIT_FAILURE);

    // while(1) {
    //     char cmd[100] = {0};
    //     int size = read(mInFd, cmd, sizeof(cmd));
    //     if (size == -1) {
    //         SPR_LOGE("Read %d cmd failed, %s", mInFd, strerror(errno));
    //     }
    //     SPR_LOGD("Read cmd: %s, %d", cmd, size);
    //     // size = write(mOutFd, "ack", 3);
    //     // if (size == -1) {
    //     //     SPR_LOGE("Write %d cmd failed, %s", mOutFd, strerror(errno));
    //     // }
    //     sleep(1);
    // }
}
