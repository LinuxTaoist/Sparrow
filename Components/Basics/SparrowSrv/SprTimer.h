/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprTimer.h
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
#ifndef __SPR_TIMER_H__
#define __SPR_TIMER_H__

#include <stdint.h>

class SprTimer
{
public:
    SprTimer(uint32_t moduleId, uint32_t msgId, uint32_t repeatTimes, uint32_t delayInMilliSec, uint32_t intervalInMilliSec);
    SprTimer(const SprTimer& timer);
    SprTimer& operator = (const SprTimer& timer);
    SprTimer(SprTimer&& timer);
    SprTimer& operator = (SprTimer&& timer);
    ~SprTimer();

    bool operator < (const SprTimer& t) const;  // used by SprTimerManager::mTimers insert
    bool IsExpired() const;
    uint64_t GetTickMs() const;
    uint32_t GetModuleId() const { return mModuleId; }
    uint32_t GetMsgId() const { return mMsgId; }
    uint32_t GetIntervalInMilliSec() const { return mIntervalInMilliSec; }
    uint64_t GetExpired() const { return mExpired; }
    uint32_t GetRepeatTimes() const { return mRepeatTimes; }
    uint32_t GetRepeatCount() const { return mRepeatCount; }

    void SetExpired(uint32_t expired) { mExpired = expired; }
    void RepeatCount() const { mRepeatCount++; }

private:
    uint32_t mModuleId;             // module id
    uint32_t mMsgId;                // msg id
    uint32_t mIntervalInMilliSec;   // interval in milliseconds
    uint64_t mExpired;              // trigger time in milliseconds
    uint32_t mRepeatTimes;          // limited repeat times, 0 means infinite
    mutable uint32_t mRepeatCount;  // repeat count
};

#endif  // __SPR_TIMER_H__