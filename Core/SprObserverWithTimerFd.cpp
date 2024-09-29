/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprObserverWithTimerfd.cpp
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
#include "SprLog.h"
#include "SprEpollSchedule.h"
#include "SprObserverWithTimerfd.h"

#define SPR_LOGD(fmt, args...)  LOGD("SprObsTmrFd", "[%s] " fmt, ##args)
#define SPR_LOGW(fmt, args...)  LOGW("SprObsTmrFd", "[%s] " fmt, ##args)
#define SPR_LOGE(fmt, args...)  LOGE("SprObsTmrFd", "[%s] " fmt, ##args)

SprObserverWithTimerfd::SprObserverWithTimerfd(ModuleIDType id, const std::string& name, InternalDefs::EProxyType proxyType)
    : SprObserver(id, name, proxyType)
{
}

SprObserverWithTimerfd::~SprObserverWithTimerfd()
{
    SprEpollSchedule::GetInstance()->DelPoll(this);
}

int32_t SprObserverWithTimerfd::InitFramework()
{
    // SPR_LOGD("AddPoll timerfd observer! fd = %d\n", GetEpollFd());
    SprEpollSchedule::GetInstance()->AddPoll(this);
    return 0;
}

void* SprObserverWithTimerfd::EpollEvent(int fd, EpollType eType, void* arg)
{
    if (fd != GetEpollFd()) {
        SPR_LOGW("fd is not timerfd!\n");
        return nullptr;
    }

    std::string bytes;
    Read(fd, bytes);
    ProcessTimerEvent();
    return nullptr;
}


