/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwPowerDefault.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/07/27
 *
 *  Default (stub) implementation of IHwPower.  All methods return HW_ERR_NOT_IMPL.
 *  Real platforms override this in Hardware/<Family>/.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __HW_POWER_DEFAULT_H__
#define __HW_POWER_DEFAULT_H__

#include <string>
#include "IHwPower.h"

class HwPowerDefault : public IHwPower {
public:
    HwPowerDefault() = default;
    ~HwPowerDefault() override = default;

    std::string GetVersion() const override;
    int32_t Init() override;
    int32_t Deinit() override;
    int32_t SetMode(EHwPowerMode mode) override;
    int32_t GetMode(EHwPowerMode& mode) override;
    int32_t GetWakeupSource(EHwWakeupSource& source) override;
    int32_t Reboot(const std::string& reason) override;
    int32_t RegisterWakeupSourceHandler(HwPowerWakeupSourceHandler handler) override;
    int32_t RegisterWakeupEdgeHandler(HwPowerWakeupEdgeHandler handler) override;
    std::string GetPlatformName() const override;
};

#endif // __HW_POWER_DEFAULT_H__
