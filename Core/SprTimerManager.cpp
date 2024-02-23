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
#include "SprTimerManager.h"

SprTimerManager::SprTimerManager()
{

}

SprTimerManager::~SprTimerManager()
{

}

int SprTimerManager::Init(void)
{
    return 0;
}

SprTimer SprTimerManager::AddTimer(uint32_t moduleId, uint32_t msgId, uint32_t repeatTimes, int32_t delayInMilliSec, int32_t intervalInMilliSec)
{
    SprTimer t(moduleId, msgId, repeatTimes, delayInMilliSec, intervalInMilliSec);
    mTimers.insert(t);

    return t;
}

int SprTimerManager::DelTimer(const SprTimer& t)
{
    auto it = mTimers.find(t);
    if (it != mTimers.end())
    {
        mTimers.erase(it);
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

time_t SprTimerManager::NextExpireTimes()
{
    return 0;
}

int SprTimerManager::TimerLoop()
{
    return 0;
}
