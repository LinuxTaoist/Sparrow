/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwWatchdogDefault.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/07/27
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __HW_WATCHDOG_DEFAULT_H__
#define __HW_WATCHDOG_DEFAULT_H__

#include <string>
#include "IHwWatchdog.h"

class HwWatchdogDefault : public IHwWatchdog {
public:
    HwWatchdogDefault() = default;
    ~HwWatchdogDefault() override = default;

    std::string GetVersion() const override;
    int32_t Start(int32_t timeoutSec) override;
    int32_t Feed() override;
    int32_t Stop() override;
};

#endif // __HW_WATCHDOG_DEFAULT_H__
