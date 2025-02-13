/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprTimer.cpp
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
#include <time.h>
#include <stdint.h>
#include <string.h>
#include "SprTimer.h"
#include "SprLog.h"

#define SPR_LOGD(fmt, args...) LOGD("SprTimer", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("SprTimer", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("SprTimer", fmt, ##args)

SprTimer::SprTimer(uint32_t moduleId, uint32_t msgId, uint32_t repeatTimes, uint32_t delayInMilliSec, uint32_t intervalInMilliSec)
{
    mModuleId = moduleId;
    mMsgId = msgId;
    mIntervalInMilliSec = intervalInMilliSec;
    mExpired = GetTick() + delayInMilliSec + intervalInMilliSec;
    mRepeatTimes = repeatTimes;
    mRepeatCount = 0;
}

SprTimer::SprTimer(const SprTimer& timer)
{
    mModuleId = timer.mModuleId;
    mMsgId = timer.mMsgId;
    mIntervalInMilliSec = timer.mIntervalInMilliSec;
    mExpired = timer.mExpired;
    mRepeatTimes = timer.mRepeatTimes;
    mRepeatCount = timer.mRepeatCount;
}

SprTimer& SprTimer::operator = (const SprTimer& timer)
{
    if (this != &timer) {
        mModuleId = timer.mModuleId;
        mMsgId = timer.mMsgId;
        mIntervalInMilliSec = timer.mIntervalInMilliSec;
        mExpired = timer.mExpired;
        mRepeatTimes = timer.mRepeatTimes;
        mRepeatCount = timer.mRepeatCount;
    }
    return *this;
}

SprTimer::SprTimer(SprTimer&& timer)
{
    mModuleId = timer.mModuleId;
    mMsgId = timer.mMsgId;
    mIntervalInMilliSec = timer.mIntervalInMilliSec;
    mExpired = timer.mExpired;
    mRepeatTimes = timer.mRepeatTimes;
    mRepeatCount = timer.mRepeatCount;
}

SprTimer& SprTimer::operator = (SprTimer&& timer)
{
    mModuleId = timer.mModuleId;
    mMsgId = timer.mMsgId;
    mIntervalInMilliSec = timer.mIntervalInMilliSec;
    mExpired = timer.mExpired;
    mRepeatTimes = timer.mRepeatTimes;
    mRepeatCount = timer.mRepeatCount;
    return *this;
}

SprTimer::~SprTimer()
{
}

bool SprTimer::operator < (const SprTimer& timer) const
{
    if (mExpired < timer.mExpired) {
        return true;
    } else if (mExpired > timer.mExpired) {
        return false;
    } else {
        return (mMsgId < timer.mMsgId);
    }
}

bool SprTimer::IsExpired() const
{
    return (GetTick() >= mExpired);
}

uint32_t SprTimer::GetTick() const
{
    uint32_t td = 0;
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    td = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

    return td;
}
