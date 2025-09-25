/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PtyTerminal.h
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
#ifndef __PTY_TERMINAL_H__
#define __PTY_TERMINAL_H__

#include <stdint.h>

class PtyTerminal
{
public:
    PtyTerminal();
    ~PtyTerminal();

    int32_t Init(int32_t clientFd);

private:
    void SetNonBlock(int32_t fd);
    ssize_t FilterColorCode(char* input, ssize_t len, char* output);

private:
    int32_t mClientFd;
    int32_t mMasterFd;
    int32_t mSlaveFd;
};

#endif // __PTY_TERMINAL_H__
