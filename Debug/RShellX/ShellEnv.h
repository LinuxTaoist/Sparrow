/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ShellEnv.h
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
#ifndef __SHELL_ENVIRONMET_H__
#define __SHELL_ENVIRONMET_H__

#include <string>

class ShellEnv
{
public:
    explicit ShellEnv(int inFd, int outFd, int errFd);
    ~ShellEnv();

    int Execute(const std::string& cmd);

private:
    // int mInFd;
    // int mOutFd;
    // int mErrFd;
    // int mStdin;     // save stdin fd
    // int mStdout;    // save stdout fd
    // int mStderr;    // save stderr fd
};

#endif // __SHELL_ENVIRONMET_H__
