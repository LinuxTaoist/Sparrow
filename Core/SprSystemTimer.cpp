/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprSystemTimer.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/02/25
 *
 *  The timerfd encapsulates interfaces
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/02/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/timerfd.h>
#include "SprCommonType.h"
#include "SprSystemTimer.h"
#include "SprMediatorIpcProxy.h"

using namespace InternalEnum;

#define SPR_LOGD(fmt, args...) printf("%d SysTimer D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%d SysTimer W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d SysTimer E: " fmt, __LINE__, ##args)

SprSystemTimer::SprSystemTimer(ModuleIDType id, const std::string& name)
                : SprObserver(id, name, std::make_shared<SprMediatorIpcProxy>(), false)
{
    mTimerRunning = false;
    mTimerFd = -1;
}

SprSystemTimer::~SprSystemTimer()
{

}

int SprSystemTimer::HandleEvent(int fd)
{
    // Event is triggered by timer, only notify to TimerManager
    if (fd == mTimerFd)
    {
        mTimerRunning = false;
        SprMsg timerMsg(MODULE_TIMERM, SIG_ID_SYSTEM_TIMER_NOTIFY);
        NotifyObserver(timerMsg);
    }

    return 0;
}

// Not need implement
int SprSystemTimer::ProcessMsg(const SprMsg& msg)
{
    return 0;
}

int SprSystemTimer::Init()
{
    return InitTimer();
}

int SprSystemTimer::InitTimer()
{
    mTimerFd = timerfd_create(CLOCK_REALTIME, 0);
    AddPoll(mTimerFd, IPC_TYPE_TIMERFD);
    return 0;
}

int SprSystemTimer::StartTimer(uint32_t intervalInMilliSec)
{
    if (mTimerRunning) {
        return 0;
    }

    struct itimerspec its;
    its.it_value.tv_sec = intervalInMilliSec / 1000;
    its.it_value.tv_nsec = (intervalInMilliSec % 1000) * 1000000;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    if (timerfd_settime(mTimerFd, 0, &its, NULL) == -1) {
        SPR_LOGE("timerfd_settime fail! (%s)\n", strerror(errno));
        return -1;
    }

    mTimerRunning = true;
    return 0;
}

int SprSystemTimer::StopTimer()
{
    struct itimerspec its;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    if (timerfd_settime(mTimerFd, 0, &its, NULL) == -1) {
        SPR_LOGE("timerfd_settime fail! (%s)\n", strerror(errno));
        return -1;
    }

    mTimerRunning = false;
    return 0;
}

int SprSystemTimer::DestoryTimer()
{
    if (mTimerFd != -1) {
        close(mTimerFd);
        mTimerFd = -1;
    }

    SPR_LOGD("Destory system timer!");
    return 0;
}
