/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprTimerManager.h
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
#ifndef __TIMER_MANAGER_H__
#define __TIMER_MANAGER_H__

#include <set>
#include <stdint.h>
#include "SprTimer.h"
#include "SprCommonType.h"

class SprTimerManager
{
public:
    SprTimerManager();
    ~SprTimerManager();

    /**
     * @brief  Init
     * @return none
     *
     * Will be called once to initialize the object instance
     */
    int Init(void);

private:
    std::set<SprTimer> mTimers;

    SprTimer AddTimer(uint32_t moduleId, uint32_t msgId, uint32_t repeatTimes, int32_t delayInMilliSec, int32_t intervalInMilliSec);
    int DelTimer(const SprTimer& t);
    int UpdateTimer();
    int CheckTimer();
    time_t NextExpireTimes();
    int TimerLoop();
};

#endif // __TIMER_MANAGER_H__
