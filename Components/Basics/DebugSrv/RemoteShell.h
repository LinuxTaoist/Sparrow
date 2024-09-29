/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : RemoteShell.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/28
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/28 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __REMOTE_SHELL_H__
#define __REMOTE_SHELL_H__

#include <memory>
#include <thread>
#include "PSocket.h"
#include "EpollEventHandler.h"

class RemoteShell
{
public:
    RemoteShell();
    ~RemoteShell();

    int Init();
    int DeInit();
    int Enable();
    int Disable();

private:
    bool mEnable;
    std::thread mRcvThread;
    std::shared_ptr<PSocket> mTcpSrvPtr;
};

#endif // __REMOTE_SHELL_H__
