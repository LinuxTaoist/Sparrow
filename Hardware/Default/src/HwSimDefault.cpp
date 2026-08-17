/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwSimDefault.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Default stub for IHwSim.
 *  @date       : 2026/08/13
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/08/13 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "HwCommonTypes.h"
#include "HwSimDefault.h"

#define HW_SIM_DEFAULT_VERSION  "HW_SIM_DEFAULT_R1001"

std::string HwSimDefault::GetVersion() const {
    return HW_SIM_DEFAULT_VERSION;
}

int32_t HwSimDefault::GetImsi(EHwSimSlot, std::string&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwSimDefault::GetIccid(EHwSimSlot, std::string&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwSimDefault::GetPhoneNum(EHwSimSlot, std::string&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwSimDefault::GetCardInfo(EHwSimSlot, SHwSimCardInfo&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwSimDefault::RegisterCardStatusCallback(HwSimCardStatusCb) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwSimDefault::SwitchSlot(EHwSimSlot) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwSimDefault::PowerUp(EHwSimSlot) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwSimDefault::PowerDown(EHwSimSlot) {
    return HW_ERR_NOT_IMPL;
}
