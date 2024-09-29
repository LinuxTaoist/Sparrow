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

class SprTimer
{
public:
    SprTimer(uint32_t moduleId, uint32_t msgId, uint32_t repeatTimes, uint32_t delayInMilliSec, uint32_t intervalInMilliSec);
    SprTimer(const SprTimer& timer);
    SprTimer& operator = (const SprTimer& timer);
    SprTimer(SprTimer&& timer);
    SprTimer& operator = (SprTimer&& timer);
    ~SprTimer();

    bool operator < (const SprTimer& t) const;
    bool IsExpired() const;
    uint32_t GetTick() const;
    uint32_t GetModuleId() const { return mModuleId; }
    uint32_t GetMsgId() const { return mMsgId; }
    uint32_t GetIntervalInMilliSec() const { return mIntervalInMilliSec; }
    uint32_t GetExpired() const { return mExpired; }
    uint32_t GetRepeatTimes() const { return mRepeatTimes; }
    uint32_t GetRepeatCount() const { return mRepeatCount; }

    void SetExpired(uint32_t expired) { mExpired = expired; }
    void RepeatCount() const { mRepeatCount++; }

private:
    uint32_t mModuleId;
    uint32_t mMsgId;
    uint32_t mIntervalInMilliSec;
    uint32_t mExpired;
    uint32_t mRepeatTimes;
    mutable uint32_t mRepeatCount;
};

#endif  // __SPR_TIMER_H__