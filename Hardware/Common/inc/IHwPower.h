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

class IHwPower {
public:
    virtual ~IHwPower() = default;

    virtual std::string GetVersion() const = 0;
    virtual int32_t Init() = 0;
    virtual int32_t Release() = 0;
    virtual int32_t SetMode(EHwPowerMode mode) = 0;
    virtual int32_t GetMode(EHwPowerMode& mode) = 0;
    virtual int32_t GetWakeupSource(EHwWakeupSource& source) = 0;
    /**
     * @brief Register a callback for wake-up source changes.
     *
     * @param  handler   callback object
     * @return HW_OK on success
     */
    virtual int32_t RegisterWakeupHandler(HwPowerWakeupHandler handler) = 0;
    virtual int32_t Reboot(const std::string& reason) = 0;
    virtual std::string GetPlatformName() const = 0;
};

#endif // __IHW_POWER_H__
