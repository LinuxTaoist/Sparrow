/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwDeviceDefault.cpp
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
#include "HwCommonTypes.h"
#include "HwDeviceDefault.h"

#define HW_DEV_DEFAULT_VERSION  "HW_DEV_DEFAULT_R1001"

std::string HwDeviceDefault::GetVersion() const {
    return HW_DEV_DEFAULT_VERSION;
}

int32_t HwDeviceDefault::SetRadio(bool) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwDeviceDefault::SetAirplaneMode(bool) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwDeviceDefault::GetAirplaneMode(EHwAirplaneMode&) {
    return HW_ERR_NOT_IMPL;
}
int32_t HwDeviceDefault::GetModemState(EHwModemState&) {
    return HW_ERR_NOT_IMPL;
}
int32_t HwDeviceDefault::RegisterAirPlaneModeCallback(HwAirPlaneModeCb) {
    return HW_ERR_NOT_IMPL;
}
