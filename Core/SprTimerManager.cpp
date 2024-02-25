/**
  *--------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprTimerManager.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/12/15
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/12/15 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timerfd.h>
#include "SprMsg.h"
#include "Util/Shared.h"
#include "SprTimerManager.h"

using namespace std;
using namespace InternalEnum;

#define SPR_LOGD(fmt, args...) printf("%d TimerM D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%d TimerM W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d TimerM E: " fmt, __LINE__, ##args)

#define TIMER_MIN_PRECISION_MS 10

SprTimerManager::SprTimerManager(ModuleIDType id, const std::string& name, shared_ptr<SprMediatorProxy> mediatorPtr)
                : SprObserver(id, name, mediatorPtr)
{
    mSystemTimerRunning = false;
    mSystemTimerFd = -1;
    // mSystemTimerId = -1;
}

SprTimerManager::~SprTimerManager()
{

}

SprTimerManager* SprTimerManager::GetInstance(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> msgMediatorPtr)
{
    static SprTimerManager instance(id, name, msgMediatorPtr);
    return &instance;
}

int SprTimerManager::Init(void)
{
    int ret = 0;
    ret = InitSystemTimer();

    return ret;
}

int SprTimerManager::DeInit()
{
    return 0;
}

int SprTimerManager::ProcessMsg(const SprMsg& msg)
{
    SPR_LOGD("msg.GetMsgId() = %s\n", GetSigName(msg.GetMsgId()));
    switch (msg.GetMsgId())
    {
        case SIG_ID_TIMER_ADD_ONCE_TIMER:
        {
            break;
        }

        case SIG_ID_TIMER_ADD_REPEAT_TIMER:
        break;

        case SIG_ID_TIMER_ADD_CUSTOM_TIMER:
        {
            MsgRespondAddTimer(msg);
            break;
        }

        case SIG_ID_TIMER_START_SYSTEM_TIMER:
        {
            MsgRespondStartSystemTimer(msg);
            break;
        }

        case SIG_ID_TIMER_STOP_SYSTEM_TIMER:
        break;

        case SIG_ID_TIMER_DEL:
        break;

        default:
            break;
    }

    return 0;
}

int SprTimerManager::HandleOtherPollEvent(uint32_t listenType)
{
    switch(listenType)
    {
        case POLL_SCHEDULE_TYPE_TIMER:
        {
            HanleTimerEvent();
            break;
        }

        default:
            break;
    }

    return 0;
}

int SprTimerManager::PrintRealTime()
{
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);

    time_t seconds = currentTime.tv_sec;
    struct tm* localTime = localtime(&seconds);
    int milliseconds = currentTime.tv_nsec / 1000000;

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    SPR_LOGD("Timestamp: %s.%03d\n", buffer, milliseconds);

    return 0;
}

int SprTimerManager::AddTimer(uint32_t moduleId, uint32_t msgId, uint32_t repeatTimes, int32_t delayInMilliSec, int32_t intervalInMilliSec)
{
    SprTimer timer(moduleId, msgId, repeatTimes, delayInMilliSec, intervalInMilliSec);
    mTimers.insert(timer);

    return 0;
}

int SprTimerManager::AddTimer(const SprTimer& timer)
{
    mTimers.insert(timer);
    return 0;
}

int SprTimerManager::DelTimer(const SprTimer& timer)
{
    auto it = mTimers.find(timer);
    if (it != mTimers.end())
    {
        mTimers.erase(it);
    } else {
        SPR_LOGW("Not exist the timer! moduleId = 0x%x, msgId = 0x%x", timer.GetModuleId(), timer.GetMsgId());
    }

    return 0;
}

int SprTimerManager::UpdateTimer()
{
    return 0;
}

int SprTimerManager::CheckTimer()
{
    return 0;
}

uint32_t SprTimerManager::NextExpireTimes()
{
    return 0;
}

int SprTimerManager::InitSystemTimer()
{
    mSystemTimerFd = timerfd_create(CLOCK_REALTIME, 0);

    AddPoll(POLL_SCHEDULE_TYPE_TIMER, mSystemTimerFd);
    SPR_LOGD("InitSystemTimer success!\n");
    return 0;
}

int SprTimerManager::StartSystemTimer(uint32_t intervalInMilliSec)
{
    if (mSystemTimerRunning) {
        return 0;
    }

    struct itimerspec its;
    its.it_value.tv_sec = intervalInMilliSec / 1000;
    its.it_value.tv_nsec = (intervalInMilliSec % 1000) * 1000000;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    if (timerfd_settime(mSystemTimerFd, 0, &its, NULL) == -1) {
        SPR_LOGE("timerfd_settime fail! (%s)\n", strerror(errno));
        return -1;
    }

    mSystemTimerRunning = true;
    return 0;
}

int SprTimerManager::StopSystemTimer()
{
    struct itimerspec its;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    if (timerfd_settime(mSystemTimerFd, 0, &its, NULL) == -1) {
        SPR_LOGE("timerfd_settime fail! (%s)\n", strerror(errno));
        return -1;
    }

    mSystemTimerRunning = false;
    return 0;
}

int SprTimerManager::DestorySystemTimer()
{
    if (mSystemTimerFd != -1) {
        close(mSystemTimerFd);
        mSystemTimerFd = -1;
    }

    SPR_LOGD("Destory system timer!");
    return 0;
}

void SprTimerManager::HanleTimerEvent()
{
    // loop: Execute the triggered timers
    for (const auto & it : mTimers)
    {
        uint32_t tick = it.GetTick();
        if (it.GetTick() - it.GetExpired() > (TIMER_MIN_PRECISION_MS / 2))
        {
            break;
        }
        else
        {
            if ((it.GetRepeatCount() + 1) < it.GetRepeatTimes() || it.GetRepeatTimes() == 0)
            {
                SprTimer t(it);

                // loop: update timer valid expired time
                do
                {
                    t.SetExpired(it.GetIntervalInMilliSec() + it.GetTick());
                } while (t.GetExpired() < it.GetTick());

                mTimers.insert(t);
                AddTimer(t);
            }

            // handle timer event
            if (Shared::AbsValue(tick, it.GetExpired()) < (TIMER_MIN_PRECISION_MS / 2))
            {
                it.RepeatCount();
                NotifyObserver(it.GetModuleId(), it.GetMsgId());

                // debug
                PrintRealTime();
            }

            DelTimer(it);
        }
    }

    if (mTimers.empty())
    {
        StopSystemTimer();
    }
}

void SprTimerManager::MsgRespondStartSystemTimer(const SprMsg &msg)
{
    auto timerNode = mTimers.begin();
    uint32_t timeValueInMilliSec = timerNode->GetExpired() - timerNode->GetTick();
    StartSystemTimer(timeValueInMilliSec);
}

void SprTimerManager::MsgRespondStopSystemTimer(const SprMsg &msg)
{

}

void SprTimerManager::MsgRespondAddTimer(const SprMsg &msg)
{
    std::shared_ptr<STimerInfo> pTimerInfo = msg.GetDatas<STimerInfo>();
    if (pTimerInfo != nullptr)
    {
        AddTimer(pTimerInfo->moduleId, pTimerInfo->msgId, pTimerInfo->repeatTimes, pTimerInfo->delayInMilliSec, pTimerInfo->intervalInMilliSec);

        SprMsg msg(SIG_ID_TIMER_START_SYSTEM_TIMER);
        SendMsg(msg);
    }

    SPR_LOGD("Add Timer! \n");
}

void SprTimerManager::MsgRespondDelTimer(const SprMsg &msg)
{

}

