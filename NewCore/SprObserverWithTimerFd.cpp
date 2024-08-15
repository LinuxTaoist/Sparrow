/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprObserverWithTimerFd.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/15
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/08/15 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "SprObserverWithTimerFd.h"

SprObserverWithTimerFd::SprObserverWithTimerFd(ModuleIDType id, const std::string& name, InternalDefs::EProxyType proxyType)
    : SprObserver(id, name, proxyType)
{
}

SprObserverWithTimerFd::~SprObserverWithTimerFd()
{
}

void* SprObserverWithTimerFd::EpollEvent(int fd, EpollType eType, void* arg)
{
    if (fd != GetEpollFd()) {
        return nullptr;
    }

    ProcessTimerEvent();
    return nullptr;
}


