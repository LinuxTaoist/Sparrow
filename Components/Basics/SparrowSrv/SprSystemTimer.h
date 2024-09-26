/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprSystemTimer.h
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

#ifndef __SPR_SYSTEM_TIMER_H__
#define __SPR_SYSTEM_TIMER_H__

#include "SprObserverWithTimerfd.h"
#include "CoreTypeDefs.h"

class SprSystemTimer : public SprObserverWithTimerfd
{
public:
    explicit SprSystemTimer(ModuleIDType id, const std::string& name);
    virtual ~SprSystemTimer();

    SprSystemTimer(const SprSystemTimer&) = delete;
    SprSystemTimer& operator=(const SprSystemTimer&) = delete;
    SprSystemTimer(SprSystemTimer&&) = delete;
    SprSystemTimer& operator=(SprSystemTimer&&) = delete;

    int ProcessTimerEvent() override;
};

#endif
