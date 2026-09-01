/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwPowerDefault.cpp
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
#include <string>
#include "HwCommonTypes.h"
#include "HwPowerDefault.h"

#define HW_POWER_DEFAULT_VERSION  "HW_POWER_DEFAULT_R1001"

std::string HwPowerDefault::GetVersion() const {
    return HW_POWER_DEFAULT_VERSION;
}

std::string HwPowerDefault::GetPlatformName() const {
    return "Default";
}

int32_t HwPowerDefault::Init() {
    return HW_ERR_NOT_IMPL;
}

int32_t HwPowerDefault::Deinit() {
    return HW_ERR_NOT_IMPL;
}

int32_t HwPowerDefault::SetMode(EHwPowerMode) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwPowerDefault::GetMode(EHwPowerMode&) {
     return HW_ERR_NOT_IMPL;
}

int32_t HwPowerDefault::GetWakeupSource(EHwWakeupSource&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwPowerDefault::Reboot(const std::string&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwPowerDefault::RegisterWakeupSourceCallback(HwPowerWakeupSourceCb) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwPowerDefault::RegisterWakeupEdgeCallback(HwPowerWakeupEdgeCb) {
    return HW_ERR_NOT_IMPL;
}
