/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TimeManager.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/11/11
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/11/11 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <atomic>
#include <mutex>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/timex.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "SprLog.h"
#include "TimeManager.h"
#include "SprDebugNode.h"
#include "SprThreadPool.h"
#include "SprEnumHelper.h"

using namespace std;
using namespace InternalDefs;

#define LOG_TAG "TimeMgr"
#define DEFAULT_NTP_PORT        8087
#define SYNC_TIME_TIMEOUT_SEC   4000    // 4 sec
#define SYNC_TIME_POLL_TIMEOUT  600000  // 10 min
#define TIME_ADJUST_SMALL_SEC   4
#define TIME_ADJUST_LARGE_SEC   120

static std::atomic<bool> gObjAlive(true);

TimeManager::TimeManager(ModuleIDType id, const std::string& name)
            : SprObserverWithMQueue(id, name)
{
    mSyncTimeFinished = false;
    mCurPriority = TIME_SOURCE_PRIORITY_BUTT;
    mCurTimeSource = TIME_SOURCE_TYPE_BUTT;

    mTimeSourceMap.insert(make_pair(TIME_SOURCE_PRIORITY_HIGH, TIME_SOURCE_TYPE_GNSS));
    mTimeSourceMap.insert(make_pair(TIME_SOURCE_PRIORITY_MEDIUM, TIME_SOURCE_TYPE_NTP));
}

TimeManager::~TimeManager()
{
    gObjAlive = false;
    UnregisterDebugFuncs();
}

TimeManager* TimeManager::GetInstance(ModuleIDType id, const std::string& name)
{
    if (!gObjAlive) {
        return nullptr;
    }

    static TimeManager instance(id, name);
    return &instance;
}

int32_t TimeManager::InitNtpSource()
{
    if (!mpNtpSource) {
        mpNtpSource = make_shared<NtpSource>(DEFAULT_NTP_PORT, [&](uint64_t timestamp, void* arg) {
            SPR_LOGD("Receive ntp time: %llu.%llu", (timestamp >> 32) & 0xFFFFFFFF, timestamp & 0xFFFFFFFF);

            TimeManager* mySelf = static_cast<TimeManager*>(arg);
            if (!mySelf) {
                SPR_LOGE("mySelf is nullptr!");
                return;
            }

            SprMsg msg(SIG_ID_TIMEM_SYNC_SYSTEM_TIME);
            msg.SetI32Value((int32_t)TIME_SOURCE_TYPE_NTP);
            msg.SetU64Value(timestamp);
            mySelf->SendMsg(msg);
        }, this);
    }

    if (!mpNtpSource) {
        SPR_LOGE("Create ntp source failed!");
        return -1;
    }

    mpNtpSource->AddNtpServer("ntp.tencent.com", 123);
    mpNtpSource->AddNtpServer("ntp1.aliyun.com", 123);
    return 0;
}

int32_t TimeManager::GetDiffWithLocalTime(uint64_t timestamp, int64_t& diffNs)
{
    struct timespec current_ts;
    if (clock_gettime(CLOCK_REALTIME, &current_ts) == -1) {
        SPR_LOGE("Get time failed! (%s)\n", strerror(errno));
        return -1;
    }

    uint64_t targetNs = ((timestamp >> 32) & 0xFFFFFFFF) * 1000000000LL + (timestamp & 0xFFFFFFFF);
    uint64_t currentNs = current_ts.tv_sec * 1000000000LL + current_ts.tv_nsec;

    diffNs = std::abs(static_cast<int64_t>(targetNs - currentNs));
    // SPR_LOGD("Time diff: %lld ns", diffNs);
    return 0;
}

int32_t TimeManager::StartSyncTime()
{
    SPR_LOGD("Start sync time\n");

    // sync time from first priority configured
    mSyncTimeFinished = false;
    mCurPriority = mTimeSourceMap.empty() ? TIME_SOURCE_PRIORITY_BUTT : mTimeSourceMap.begin()->first;
    return RegisterTimer(0, SYNC_TIME_TIMEOUT_SEC,  SIG_ID_TIMEM_SYNC_TIME_TIMER_EVENT, 0);
}

int32_t TimeManager::StopSyncTime()
{
    SPR_LOGD("Stop sync time\n");
    mCurPriority = TIME_SOURCE_PRIORITY_BUTT;
    return UnregisterTimer(SIG_ID_TIMEM_SYNC_TIME_TIMER_EVENT);
}

int32_t TimeManager::StartSyncTimePoller()
{
    return RegisterTimer(0, SYNC_TIME_POLL_TIMEOUT, SIG_ID_TIMEM_SYNC_TIME_POLL_TIMER_EVENT, 0);
}

int32_t TimeManager::StopSyncTimePoller()
{
    return UnregisterTimer(SIG_ID_TIMEM_SYNC_TIME_POLL_TIMER_EVENT);
}

int32_t TimeManager::SmoothAdjustSystemTime(int64_t ns)
{
    int32_t us = static_cast<int32_t>(ns / 1000);
    struct timex tx = {};
    tx.modes = ADJ_OFFSET;
    tx.offset = static_cast<int32_t>(us);

    int32_t ret = adjtimex(&tx);
    if (ret == -1) {
        SPR_LOGE("Smooth adjust failed: %s (adjust_us: %d)\n", strerror(errno), us);
        return -1;
    }

    SPR_LOGI("Smooth adjust %lld ns (%d us) successfully\n", ns, us);
    return 0;
}

int32_t TimeManager::JumpAdjustSystemTime(uint64_t timestamp)
{
    struct timespec ts;
    ts.tv_sec = (timestamp >> 32) & 0xFFFFFFFF;
    ts.tv_nsec = timestamp & 0xFFFFFFFF;

    int32_t ret = clock_settime(CLOCK_REALTIME, &ts);
    if (ret == -1) {
        SPR_LOGE("Set system time failed: %s\n", strerror(errno));
        return -1;
    }

    SPR_LOGI("Jump adjust %lu successfully\n", ts.tv_sec);
    return 0;
}

int32_t TimeManager::SyncSystemTime(int32_t source, uint64_t timestamp)
{
    if (source < TIME_SOURCE_TYPE_NTP || source >= TIME_SOURCE_TYPE_BUTT) {
        SPR_LOGE("Invalid time source: %d\n", source);
        return -1;
    }

    int64_t diffNs = 0;
    int32_t ret = GetDiffWithLocalTime(timestamp, diffNs);
    if (ret == -1) {
        return -1;
    }

    int64_t diffSec = diffNs / 1000000000LL;
    if (diffSec <= TIME_ADJUST_SMALL_SEC)    {
        SPR_LOGD("Not need sync time! (small time difference %lld sec)\n", diffSec);
    } else if (diffSec <= TIME_ADJUST_LARGE_SEC) {
        SmoothAdjustSystemTime(diffNs);
    } else {
        JumpAdjustSystemTime(timestamp);
    }

    mSyncTimeFinished = true;
    mCurTimeSource = (InternalDefs::TimeSourceType)source;
    return ret;
}

TimeSourcePriority TimeManager::GetTimeSourcePriority(InternalDefs::TimeSourceType source)
{
    TimeSourcePriority ret = TIME_SOURCE_PRIORITY_BUTT;
    for (auto it = mTimeSourceMap.begin(); it != mTimeSourceMap.end(); it++) {
        if (it->second == source) {
            ret = it->first;
            break;
        }
    }

    return ret;
}

InternalDefs::TimeSourceType TimeManager::GetTimeSource(TimeSourcePriority priority)
{
    auto it = mTimeSourceMap.find(priority);
    if (it != mTimeSourceMap.end()) {
        return it->second;
    }

    return TIME_SOURCE_TYPE_BUTT;
}

int32_t TimeManager::Init()
{
    RegisterDebugFuncs();
    InitNtpSource();
    return 0;
}

int32_t TimeManager::RequestNtpTime()
{
    if (!mpNtpSource) {
        SPR_LOGE("Ntp client is nullptr!");
        return -1;
    }

    SPR_LOGD("Start request ntp time");
    SprThreadPool::GetInstance(2)->SubmitTask([&]() {
        // long time to send request, so run it in thread pool
        mpNtpSource->SendTimeRequest();
    });
    SPR_LOGD("Request ntp time ret = %d\n", 0);
    return 0;
}

int32_t TimeManager::RequestGnssTime()
{
    return -1;
}

int32_t TimeManager::ProcessMsg(const SprMsg& msg)
{
    switch(msg.GetMsgId())
    {
        case SIG_ID_TIMEM_SYNC_TIME_TIMER_EVENT:
            MsgRespondSyncTimeTimerEvent(msg);
            break;
        case SIG_ID_TIMEM_REQ_NTP_TIME:
            MsgRespondRequestNtpTime(msg);
            break;
        case SIG_ID_TIMEM_SYNC_SYSTEM_TIME:
            MsgRespondSyncSystemTime(msg);
            break;
        case SIG_ID_TIMEM_SYNC_TIME_POLL_TIMER_EVENT:
            MsgRespondSyncTimePollerTimerEvent(msg);
            break;
        default:
            break;
    }

    return 0;
}

/**
 * @brief Process SIG_ID_TIMEM_SYNC_TIME_TIMER_EVENT
 *
 * @param[in] msg
 * @return none
 */
void TimeManager::MsgRespondSyncTimeTimerEvent(const SprMsg& msg)
{
    if (mSyncTimeFinished || mCurPriority >= TIME_SOURCE_PRIORITY_BUTT) {
        UnregisterTimer(SIG_ID_TIMEM_SYNC_TIME_TIMER_EVENT);
        SPR_LOGD("Sync time finished %d, priority %d\n", mSyncTimeFinished, mCurPriority);
        return;
    }

    int32_t ret = -1;
    InternalDefs::TimeSourceType source = GetTimeSource((TimeSourcePriority)mCurPriority);
    switch(source) {
        case TIME_SOURCE_TYPE_NTP:
            ret = RequestNtpTime();
            break;
        case TIME_SOURCE_TYPE_GNSS:
            ret = RequestGnssTime();
            break;
        default:
            break;
    }

    // if ret is -1, means the time source is not available, try next priority
    mCurPriority = (ret == -1) ? mCurPriority + 1 : mCurPriority;
    SPR_LOGD("Syncing time, priority = %d, source = %s\n", mCurPriority, GetSprTimeSourceTypeText(source).c_str());
}

/**
 * @brief Process SIG_ID_TIMEM_REQ_NTP_TIME
 *
 * @param[in] msg
 * @return none
 */
void TimeManager::MsgRespondRequestNtpTime(const SprMsg& msg)
{
    SPR_LOGD("Request ntp time\n");
    RequestNtpTime();
}

/**
 * @brief Process SIG_ID_TIMEM_SYNC_SYSTEM_TIME
 *
 * @param[in] msg
 * @return none
 */
void TimeManager::MsgRespondSyncSystemTime(const SprMsg& msg)
{
    int32_t source = msg.GetI32Value();
    uint64_t timestamp = msg.GetU64Value();
    int32_t ret = SyncSystemTime(source, timestamp);
    SPR_LOGD("Sync time from %s %s\n", GetSprTimeSourceTypeText(source).c_str(), ret == -1 ? "failed" : "success");
}

/**
 * @brief Process SIG_ID_TIMEM_SYNC_TIME_POLL_TIMER_EVENT
 *
 * @param[in] msg
 * @return none
 */
void TimeManager::MsgRespondSyncTimePollerTimerEvent(const SprMsg& msg)
{
    SPR_LOGD("Receive sync time poller timer event\n");
    StartSyncTime();
}

// --------------------------------------------------------------------------------------------------------------------
// Debug functions
// --------------------------------------------------------------------------------------------------------------------
void TimeManager::RegisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    p->RegisterCmd(mModuleName, "ReqNtpTime",       "Request ntp time",    std::bind(&TimeManager::DebugRequestNtpTime,        this, std::placeholders::_1));
    p->RegisterCmd(mModuleName, "StartSyncTime",    "Start sync time",     std::bind(&TimeManager::DebugStartSyncTime,         this, std::placeholders::_1));
    p->RegisterCmd(mModuleName, "StopSyncTime",     "Stop sync time",      std::bind(&TimeManager::DebugStopSyncTime,          this, std::placeholders::_1));
    p->RegisterCmd(mModuleName, "StartPoll",        "Start time poll",     std::bind(&TimeManager::DebugStartSyncTimePoller,   this, std::placeholders::_1));
    p->RegisterCmd(mModuleName, "StopPoll",         "Stop time poll",      std::bind(&TimeManager::DebugStopSyncTimePoller,    this, std::placeholders::_1));
}

void TimeManager::UnregisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    SPR_LOGD("Unregister %s all debug funcs\n", mModuleName.c_str());
    p->UnregisterCmd(mModuleName);
}

void TimeManager::DebugStartSyncTime(const std::vector<std::string>& args)
{
    StartSyncTime();
}

void TimeManager::DebugStopSyncTime(const std::vector<std::string>& args)
{
    StopSyncTime();
}

void TimeManager::DebugStartSyncTimePoller(const std::vector<std::string>& args)
{
    StartSyncTimePoller();
}

void TimeManager::DebugStopSyncTimePoller(const std::vector<std::string>& args)
{
    StopSyncTimePoller();
}

void TimeManager::DebugRequestNtpTime(const std::vector<std::string>& args)
{
    SPR_LOGD("Debug request ntp time\n");
    RequestNtpTime();
}
