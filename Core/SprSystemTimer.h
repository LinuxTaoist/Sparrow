/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprSystemTimer.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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

#ifndef __SPR_SYSTEM_TIMER_H__
#define __SPR_SYSTEM_TIMER_H__

#include "SprObserver.h"
#include "SprCommonType.h"

class SprSystemTimer : public SprObserver
{
public:
    SprSystemTimer(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mediatorPtr);
    ~SprSystemTimer();
    SprSystemTimer(const SprSystemTimer&) = delete;
    SprSystemTimer& operator=(const SprSystemTimer&) = delete;
    SprSystemTimer(SprSystemTimer&&) = delete;
    SprSystemTimer& operator=(SprSystemTimer&&) = delete;

    int ProcessMsg(const SprMsg& msg);

    int Init();
    int InitTimer();
    int StartTimer(uint32_t intervalInMilliSec);
    int StopTimer();
    int DestoryTimer();

private:
    bool mTimerRunning;
    int  mTimerFd;
};

#endif
