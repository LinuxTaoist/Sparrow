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

#include <string>
#include <memory>
#include <stdint.h>
#include <PPipe.h>

class PtyTerminal
{
public:
    explicit PtyTerminal(const std::function<void(int32_t ret, std::string, void*)>& cb = nullptr, void* arg = nullptr);
    ~PtyTerminal();

    int32_t Init();
    int32_t Write(const std::string& bytes);

private:
    int32_t BashProcess();
    int32_t MasterProcess();
    int32_t EraseColor(const std::string& in, std::string& out);
    std::string GetCurShell();

private:
    void* mArg;
    int32_t mMasterFd;
    int32_t mSlaveFd;
    std::shared_ptr<PPipe> mPtyPipe;
    std::function<void(int32_t, std::string, void*)> mCb;
};

#endif // __PTY_TERMINAL_H__
