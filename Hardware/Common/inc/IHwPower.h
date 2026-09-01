/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IHwPower.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/07/27
 *
 *  Power management HAL interface.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __IHW_POWER_H__
#define __IHW_POWER_H__

#include <cstdint>
#include <string>
#include "HwCommonTypes.h"

using HwPowerWakeupSourceCb = std::function<void(EHwWakeupSource source)>;
using HwPowerWakeupEdgeCb   = std::function<void(EHwLpmEdge edge)>;

class IHwPower {
public:
    virtual ~IHwPower() = default;

    virtual std::string GetVersion() const = 0;
    virtual int32_t Init() = 0;
    virtual int32_t Deinit() = 0;
    virtual int32_t SetMode(EHwPowerMode mode) = 0;
    virtual int32_t GetMode(EHwPowerMode& mode) = 0;
    virtual int32_t GetWakeupSource(EHwWakeupSource& source) = 0;
    virtual int32_t Reboot(const std::string& reason) = 0;
    virtual int32_t RegisterWakeupSourceCallback(HwPowerWakeupSourceCb cb) = 0;
    virtual int32_t RegisterWakeupEdgeCallback(HwPowerWakeupEdgeCb cb) = 0;
    virtual std::string GetPlatformName() const = 0;
};

#endif // __IHW_POWER_H__
