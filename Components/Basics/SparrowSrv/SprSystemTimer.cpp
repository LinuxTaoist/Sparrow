/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprSystemTimer.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/02/25
 *
 *  The timerfd encapsulates interfaces
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/02/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/timerfd.h>
#include "SprLog.h"
#include "CoreTypeDefs.h"
#include "SprSystemTimer.h"

using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("SysTimer", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("SysTimer", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("SysTimer", fmt, ##args)

SprSystemTimer::SprSystemTimer(ModuleIDType id, const std::string& name)
    : SprObserverWithTimerfd(id, name)
{
}

SprSystemTimer::~SprSystemTimer()
{
}

int SprSystemTimer::ProcessTimerEvent()
{
    // Event is triggered by timer, only notify to TimerManager
    SprMsg timerMsg(MODULE_TIMERM, SIG_ID_SYSTEM_TIMER_NOTIFY);
    return NotifyObserver(timerMsg);
}
