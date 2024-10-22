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
#include "GeneralUtils.h"
#include "ShellEnv.h"

#define SPR_LOGD(fmt, args...) LOGD("RShellEnv", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("RShellEnv", fmt, ##args)

ShellEnv::ShellEnv(int inFd, int outFd, int errFd)
{
    // mInFd = inFd;
    // mOutFd = outFd;
    // mErrFd = errFd;
    // mStdin = dup(STDIN_FILENO);
    // mStdout = dup(STDOUT_FILENO);
    // mStderr = dup(STDERR_FILENO);
    // dup2(inFd, STDIN_FILENO);
    // dup2(outFd, STDOUT_FILENO);
    // dup2(errFd, STDERR_FILENO);
}

ShellEnv::~ShellEnv()
{
    // dup2(mStdin, STDIN_FILENO);
    // dup2(mStdout, STDOUT_FILENO);
    // dup2(mStderr, STDERR_FILENO);
    // close(mInFd);
    // close(mOutFd);
    // close(mErrFd);
    SPR_LOGD("Exit shell environment");
}

int ShellEnv::Execute(const std::string& cmd)
{
    size_t pos = cmd.find(' ');
    std::string cmdName = cmd.substr(0, pos);
    std::vector<std::string> tmpArgs = GeneralUtils::Split(cmd, ' ');
    char* args[tmpArgs.size() + 1];
    args[tmpArgs.size()] = nullptr;
    for (size_t i = 0; i < tmpArgs.size(); i++) {
        args[i] = const_cast<char*>(tmpArgs[i].c_str());
    }

    pid_t pid = fork();
    if (pid == 0) {
        int rc = execvp(cmdName.c_str(), args);
        if (rc < 0) {
            SPR_LOGE("execlp failed: %s\n", strerror(errno));
        }
        _exit(EXIT_FAILURE);
    } else {
        SPR_LOGD("ppid = %d, cpid = %d cmd: %s\n", getpid(), pid, cmd.c_str());
    }

    return pid;
}
