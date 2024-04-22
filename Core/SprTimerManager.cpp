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
#include "GeneralUtils.h"
#include "CommonMacros.h"
#include "SprTimerManager.h"
#include "SprMediatorIpcProxy.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("TimerM", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("TimerM", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("TimerM", fmt, ##args)

#define TIMER_MIN_INTERVAL_MS 100   // 100ms

SprTimerManager::SprTimerManager(ModuleIDType id, const std::string& name, shared_ptr<SprSystemTimer> systemTimerPtr)
        : SprObserver(id, name, make_shared<SprMediatorIpcProxy>())
{
    mEnable = false;
    mSystemTimerPtr = systemTimerPtr;
}

SprTimerManager::~SprTimerManager()
{

}

SprTimerManager* SprTimerManager::GetInstance(ModuleIDType id,
                    const std::string& name, shared_ptr<SprSystemTimer> systemTimerPtr)
{
    static SprTimerManager instance(id, name, systemTimerPtr);
    return &instance;
}

int SprTimerManager::Init(void)
{
    int ret = 0;
    ret = InitSystemTimer();
    mEnable = (ret == 0) ? true : false;

    return ret;
}

int SprTimerManager::ProcessMsg(const SprMsg& msg)
{
    if (!mEnable) {
        SPR_LOGW("Disable status!\n");
    }

    SPR_LOGD("[0x%x -> 0x%x] msg.GetMsgId() = %s\n", msg.GetFrom(), msg.GetTo(), GetSigName(msg.GetMsgId()));
    switch (msg.GetMsgId())
    {
        case SIG_ID_TIMER_START_SYSTEM_TIMER:
        {
            MsgRespondStartSystemTimer(msg);
            break;
        }

        case SIG_ID_TIMER_STOP_SYSTEM_TIMER:
        {
            MsgRespondStopSystemTimer(msg);
            break;
        }

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

        case SIG_ID_TIMER_DEL_TIMER:
        {
            MsgRespondDelTimer(msg);
            break;
        }

        case SIG_ID_SYSTEM_TIMER_NOTIFY:
        {
            MsgRespondSystemTimerNotify(msg);
            break;
        }

        case SIG_ID_PROXY_BROADCAST_EXIT_COMPONENT:
        {
            MsgRespondClearTimersForExitComponent(msg);
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
    return AddTimer(timer);
}

int SprTimerManager::AddTimer(const SprTimer& timer)
{
    // SPR_LOGD("AddTimer: [0x%x %dms %s]\n", timer.GetModuleId(), timer.GetIntervalInMilliSec(), GetSigName(timer.GetMsgId()));
    mTimers.insert(timer);
    return 0;
}

int SprTimerManager::DelTimer(const SprTimer& timer)
{
    auto it = mTimers.find(timer);
    if (it != mTimers.end()) {
        mTimers.erase(it);
    } else {
        SPR_LOGW("Not exist the timer! moduleId = 0x%x, msgId = 0x%x", timer.GetModuleId(), timer.GetMsgId());
    }

    return 0;
}

uint32_t SprTimerManager::NextExpireTimes()
{
    return 0;
}

int SprTimerManager::InitSystemTimer()
{
    // systemTimer already initialized in sprSystem.Init()
    if (mSystemTimerPtr == nullptr) {
        SPR_LOGE("mSystemTimerPtr is nullptr!");
        return -1;
    }

    return 0;
}

void SprTimerManager::MsgRespondStartSystemTimer(const SprMsg &msg)
{
    auto timerNode = mTimers.begin();
    uint32_t expired = timerNode->GetExpired();
    uint32_t tick = timerNode->GetTick();
    int32_t timeValueInMilliSec = expired - tick;

    // loop: If the timer has already expired, increment the wait time by the standard interval.
    while (timeValueInMilliSec <= 0) {
        SPR_LOGW("timeValueInMilliSec <= 0! (%u) (%u), reset %u %\n", expired, tick, timeValueInMilliSec);
        timeValueInMilliSec += timerNode->GetIntervalInMilliSec();
    }

    mSystemTimerPtr->StartTimer(timeValueInMilliSec);
}

void SprTimerManager::MsgRespondStopSystemTimer(const SprMsg &msg)
{
    SPR_LOGD("SIG_ID_TIMER_STOP_SYSTEM_TIMER\n");
    mSystemTimerPtr->StopTimer();
}

void SprTimerManager::MsgRespondAddTimer(const SprMsg &msg)
{
    std::shared_ptr<STimerInfo> p = msg.GetDatas<STimerInfo>();
    if (p != nullptr) {
        SPR_LOGD("AddTimer: [0x%x %dms %dms %s]\n", p->ModuleId, p->DelayInMilliSec, p->IntervalInMilliSec, GetSigName(p->MsgId));
        // if (p->IntervalInMilliSec < TIMER_MIN_INTERVAL_MS) {
        //     SPR_LOGW("Interval in milliseconds is too small: %d !\n", p->IntervalInMilliSec);
        //     return;
        // }

        AddTimer(p->ModuleId, p->MsgId, p->RepeatTimes, p->DelayInMilliSec, p->IntervalInMilliSec);
        SprMsg rspMsg(SIG_ID_TIMER_START_SYSTEM_TIMER);
        SendMsg(rspMsg);

        // debug
        // PrintRealTime();
    }

    // SPR_LOGD("Add Timer!\n");
}

void SprTimerManager::MsgRespondDelTimer(const SprMsg &msg)
{
    std::shared_ptr<STimerInfo> p = msg.GetDatas<STimerInfo>();
    if (p != nullptr) {
        for (const auto& timer : mTimers)
        {
            if (timer.GetMsgId() == p->MsgId && timer.GetModuleId() == p->ModuleId)
            {
                DelTimer(timer);
                break;
            }
        }
    }
}

void SprTimerManager::MsgRespondSystemTimerNotify(const SprMsg &msg)
{
    set<SprTimer> deleteTimers;

    // loop: Execute the triggered timers, timers are sorted by Expired value from smallest to largest
    for (auto it = mTimers.begin(); it != mTimers.end(); ++it) {
        if (it->IsExpired()) {
            if (it->GetRepeatTimes() == 0 || (it->GetRepeatCount() + 1) < it->GetRepeatTimes()) {
                SprTimer t(*it);

                // loop: update timer valid expired time
                uint32_t tmpExpired = t.GetExpired();
                do {
                    tmpExpired += t.GetIntervalInMilliSec();
                    t.RepeatCount();
                } while (tmpExpired < it->GetTick());

                if (it->GetRepeatTimes() == 0 || (it->GetRepeatCount() + 1) < it->GetRepeatTimes()) {
                    t.SetExpired(tmpExpired);
                    AddTimer(t);
                }
            }

            // Notify expired timer event to the book component
            SprMsg bookMsg(it->GetModuleId(), it->GetMsgId());
            NotifyObserver(bookMsg);
            it->RepeatCount();
            deleteTimers.insert(*it);
        } else {
            break;
        }
    }

    // Delete expired timers
    for (const auto& timer : deleteTimers) {
        DelTimer(timer);
    }

    // Set next system timer
    uint32_t msgId = mTimers.empty() ? SIG_ID_TIMER_STOP_SYSTEM_TIMER : SIG_ID_TIMER_START_SYSTEM_TIMER;
    SprMsg sysMsg(msgId);
    SendMsg(sysMsg);
    // SPR_LOGD("Current total timers size = %d\n", (int)mTimers.size());
}

void SprTimerManager::MsgRespondClearTimersForExitComponent(const SprMsg &msg)
{
    uint32_t moduleId = msg.GetU32Value();
    set<SprTimer> deleteTimers;

    // loop: delete times of exit compnent
    for (auto it = mTimers.begin(); it != mTimers.end(); ++it) {
        if (it->GetModuleId() == moduleId) {
            deleteTimers.insert(*it);
        }
    }

    for (const auto& timer : deleteTimers) {
        DelTimer(timer);
    }
}
