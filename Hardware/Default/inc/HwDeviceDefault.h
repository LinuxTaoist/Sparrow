/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwDeviceDefault.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Default stub for IHwDevice.
 *  @date       : 2026/08/12
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/08/12 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __HW_DEVICE_DEFAULT_H__
#define __HW_DEVICE_DEFAULT_H__

#include <string>
#include "IHwDevice.h"

class HwDeviceDefault : public IHwDevice {
public:
    HwDeviceDefault() = default;
    ~HwDeviceDefault() override = default;

    std::string GetVersion() const override;
    int32_t SetRadio(bool on) override;
    int32_t SetAirplaneMode(bool on) override;
    int32_t GetAirplaneMode(EHwAirplaneMode& mode) override;
    int32_t GetModemState(EHwModemState& state) override;
    int32_t RegisterAirPlaneModeCallback(HwAirPlaneModeCb handler) override;
};

#endif // __HW_DEVICE_DEFAULT_H__
