/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprTimer.h
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
#ifndef __SPR_TIMER_H__
#define __SPR_TIMER_H__

#include <ctime>

class SprTimer
{
public:
    SprTimer(uint32_t moduleId, uint64_t msgId, uint32_t repeatTimes, uint64_t delayInMilliSec, uint64_t intervalInMilliSec);
    ~SprTimer();

    bool operator < (const SprTimer& t) const;
    uint32_t GetTick();

private:
    uint32_t mModuleId;
    uint64_t mMsgId;
    uint64_t mIntervalInMilliSec;
    uint32_t mExpired;
    uint32_t mRepeatTimes;
};

#endif  // __SPR_TIMER_H__