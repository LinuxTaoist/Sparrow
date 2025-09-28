/**
  *--------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprTimerManager.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
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
#include <atomic>
#include <algorithm>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timerfd.h>
#include "SprLog.h"
#include "SprMsg.h"
#include "SprDebugNode.h"
#include "GeneralUtils.h"
#include "SprEnumHelper.h"
#include "SprTimerManager.h"

using namespace std;
using namespace InternalDefs;

#define LOG_TAG "TimerM"
#define TIMER_MIN_INTERVAL_MS 100   // 100ms

static std::atomic<bool> gObjAlive(true);

SprTimerManager::SprTimerManager(ModuleIDType id, const std::string& name, shared_ptr<SprSystemTimer> pSystemTimer)
        : SprObserverWithMQueue(id, name)
{
    mEnable = false;
    mpSystemTimer = pSystemTimer;
}

SprTimerManager::~SprTimerManager()
{
    UnregisterDebugFuncs();
}

SprTimerManager* SprTimerManager::GetInstance(ModuleIDType id,
                    const std::string& name, shared_ptr<SprSystemTimer> pSystemTimer)
{
    if (!gObjAlive) {
        return nullptr;
    }

    static SprTimerManager instance(id, name, pSystemTimer);
    return &instance;
}

int32_t SprTimerManager::Init(void)
{
    int32_t ret = 0;
    ret = InitSystemTimer();
    mEnable = (ret == 0) ? true : false;

    RegisterDebugFuncs();
    return ret;
}

int32_t SprTimerManager::ProcessMsg(const SprMsg& msg)
{
    if (!mEnable) {
        SPR_LOGW("Disable status!\n");
    }

    // SPR_LOGD("[0x%x -> 0x%x] msg.GetMsgId() = %s\n", msg.GetFrom(), msg.GetTo(), GetSigName(msg.GetMsgId()));
    switch (msg.GetMsgId()) {
        case SIG_ID_TIMER_START_SYSTEM_TIMER: {
            MsgRespondStartSystemTimer(msg);
            break;
        }
        case SIG_ID_TIMER_STOP_SYSTEM_TIMER: {
            MsgRespondStopSystemTimer(msg);
            break;
        }
        case SIG_ID_TIMER_ADD_CUSTOM_TIMER: {
            MsgRespondAddTimer(msg);
            break;
        }
        case SIG_ID_TIMER_DEL_TIMER: {
            MsgRespondDelTimer(msg);
            break;
        }
        case SIG_ID_SYSTEM_TIMER_NOTIFY: {
            MsgRespondSystemTimerNotify(msg);
            break;
        }
        case SIG_ID_PROXY_BROADCAST_EXIT_COMPONENT: {
            MsgRespondClearTimersForExitComponent(msg);
            break;
        }
        default:
            break;
    }

    return 0;
}

int32_t SprTimerManager::PrintRealTime()
{
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);

    time_t seconds = currentTime.tv_sec;
    struct tm* localTime = localtime(&seconds);
    int32_t milliseconds = currentTime.tv_nsec / 1000000;

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    SPR_LOGD("Timestamp: %s.%03d\n", buffer, milliseconds);

    return 0;
}

bool SprTimerManager::IsExistTimer(uint32_t moduleId, uint32_t msgId)
{
    auto it = std::find_if(mTimers.begin(), mTimers.end(), [moduleId, msgId](const SprTimer& t) {
        return (t.GetModuleId() == moduleId && t.GetMsgId() == msgId);
    });

    if (it != mTimers.end()) {
        return true;
    }

    return false;
}

int32_t SprTimerManager::AddTimer(uint32_t moduleId, uint32_t msgId, uint32_t repeatTimes, int32_t delayInMilliSec, int32_t intervalInMilliSec)
{
    SprTimer timer(moduleId, msgId, repeatTimes, delayInMilliSec, intervalInMilliSec);
    return AddTimer(timer);
}

int32_t SprTimerManager::AddTimer(const SprTimer& timer)
{
    // SPR_LOGD("AddTimer: [0x%x %dms %s]\n", timer.GetModuleId(), timer.GetIntervalInMilliSec(), GetSigName(timer.GetMsgId()));
    mTimers.insert(timer);
    return 0;
}

int32_t SprTimerManager::DelTimer(const SprTimer& timer)
{
    auto it = mTimers.find(timer);
    if (it != mTimers.end()) {
        mTimers.erase(it);
    } else {
        SPR_LOGW("Not exist the timer! [%s: %s]", GetSprModuleIDText(timer.GetModuleId()).c_str(), GetSigName(timer.GetMsgId()));
    }

    return 0;
}

uint32_t SprTimerManager::NextExpireTimes()
{
    return 0;
}

int32_t SprTimerManager::InitSystemTimer()
{
    // systemTimer already initialized in sprSystem.Init()
    if (mpSystemTimer == nullptr) {
        SPR_LOGE("mpSystemTimer is nullptr!");
        return -1;
    }

    return 0;
}

void SprTimerManager::MsgRespondStartSystemTimer(const SprMsg &msg)
{
    if (mTimers.empty()) {
        SPR_LOGW("No timer exist!\n");
        return;
    }

    auto timerNode = mTimers.begin();
    uint64_t expired = timerNode->GetExpired();
    uint64_t tick = timerNode->GetTickMs();
    int32_t timerIntervalInMSec = expired - tick;

    // To deal with problems that are triggered immediately (delay = 0)
    // if immediate trigger, set timer interval to 1ms
    if (timerIntervalInMSec >= -10 && timerIntervalInMSec <= 0) {
        timerIntervalInMSec = 1;
    }

    // loop: If the timer has already expired, increment the wait time by the standard interval.
    //       retry up to 10 times
    int32_t count = 0;
    while (timerIntervalInMSec <= 0 && count < 10) {
        SPR_LOGW("timerIntervalInMSec <= 0! (%u) (%u). Reset timer interval %dms, count = %d\n",
                expired, tick, timerIntervalInMSec, count);
        count++;
        timerIntervalInMSec += timerNode->GetIntervalInMilliSec();
    }

    mpSystemTimer->StartTimer(timerIntervalInMSec);
}

void SprTimerManager::MsgRespondStopSystemTimer(const SprMsg &msg)
{
    SPR_LOGD("SIG_ID_TIMER_STOP_SYSTEM_TIMER\n");
    mpSystemTimer->StopTimer();
}

void SprTimerManager::MsgRespondAddTimer(const SprMsg &msg)
{
    // When add a new timer:
    // 1. check interval value, not less than TIMER_MIN_INTERVAL_MS
    // 2. check if the timer already exist
    // 3. add the timer to the timer list, and update the system timer from the earliest timer in the list
    auto p = msg.GetDatas<STimerInfo>();
    if (p != nullptr) {
        SPR_LOGD("AddTimer: [%s %d %dms %dms %s]\n", GetSprModuleIDText(p->moduleId).c_str(),
            p->repeatTimes, p->delayInMilliSec, p->intervalInMilliSec, GetSigName(p->msgId));

        // 1. check interval value, not less than TIMER_MIN_INTERVAL_MS
        if (p->intervalInMilliSec < TIMER_MIN_INTERVAL_MS) {
            SPR_LOGW("Interval too small (%d ms), minimum allowed is %d ms!\n", p->intervalInMilliSec, TIMER_MIN_INTERVAL_MS);
            return;
        }

        // 2. check if the timer already exist
        if (IsExistTimer(p->moduleId, p->msgId)) {
            SPR_LOGW("Timer already exist!\n");
            return;
        }

        // 3. add the timer to the timer list, and update the system timer with the earliest timer in the list
        AddTimer(p->moduleId, p->msgId, p->repeatTimes, p->delayInMilliSec, p->intervalInMilliSec);
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
    if (p == nullptr) {
        SPR_LOGW("p is nullptr!\n");
        return;
    }

    auto it = std::find_if(mTimers.begin(), mTimers.end(), [&p](const SprTimer& timer) {
        return (timer.GetMsgId() == p->msgId && timer.GetModuleId() == p->moduleId);
    });

    if (it != mTimers.end()) {
        SPR_LOGD("DelTimer: [%s %dms %s]\n", GetSprModuleIDText(it->GetModuleId()).c_str(),
            it->GetIntervalInMilliSec(), GetSigName(it->GetMsgId()));

        DelTimer(*it);
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
                uint64_t tmpExpired = t.GetExpired();
                do {
                    tmpExpired += t.GetIntervalInMilliSec();
                    t.RepeatCount();
                } while (tmpExpired < it->GetTickMs());

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
    // SPR_LOGD("Current total timers size = %d\n", (int32_t)mTimers.size());
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

// --------------------------------------------------------------------------------------------------------------------
// Debug functions
// --------------------------------------------------------------------------------------------------------------------
void SprTimerManager::RegisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    p->RegisterCmd(mModuleName, "DumpTimers",    "Dump all timers", std::bind(&SprTimerManager::DebugDumpTimers,  this, std::placeholders::_1));
}

void SprTimerManager::UnregisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    SPR_LOGD("Unregister %s all debug funcs\n", mModuleName.c_str());
    p->UnregisterCmd(mModuleName);
}

void SprTimerManager::DebugDumpTimers(const std::vector<std::string>& args)
{
    SPR_LOGI("                           Show All Timers (%d)                                                \n", (int32_t)mTimers.size());
    SPR_LOGI("-----------------------------------------------------------------------------------------------\n");
    SPR_LOGI(" MODULE  INTERVAL(ms)  DIFTIME(ms)  RTIMES  RCOUNT  MSG \n");
    SPR_LOGI("-----------------------------------------------------------------------------------------------\n");
    for (auto it = mTimers.begin(); it != mTimers.end(); ++it) {
        SPR_LOGI(" %6d  %12d  %11lld  %6d  %6d  %s", it->GetModuleId(), it->GetIntervalInMilliSec(),
            it->GetExpired() - it->GetTickMs(), it->GetRepeatTimes() % 1000000, it->GetRepeatCount() % 1000000, GetSigName(it->GetMsgId()));
    }
    SPR_LOGI("-----------------------------------------------------------------------------------------------\n");
}
