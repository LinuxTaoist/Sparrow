/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwSimDefault.h
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
#ifndef __HW_SIM_DEFAULT_H__
#define __HW_SIM_DEFAULT_H__

#include <string>
#include "IHwSim.h"

class HwSimDefault : public IHwSim {
public:
    HwSimDefault() = default;
    ~HwSimDefault() override = default;

    std::string GetVersion() const override;
    int32_t GetImsi(EHwSimSlot slot, std::string& imsi) override;
    int32_t GetIccid(EHwSimSlot slot, std::string& iccid) override;
    int32_t GetPhoneNum(EHwSimSlot slot, std::string& phone) override;
    int32_t GetCardInfo(EHwSimSlot slot, SHwSimCardInfo& info) override;
    int32_t RegisterCardStatusCallback(HwSimCardStatusCb cb) override;
    int32_t SwitchSlot(EHwSimSlot phySlot) override;
    int32_t PowerUp(EHwSimSlot slot) override;
    int32_t PowerDown(EHwSimSlot slot) override;
};

#endif // __HW_SIM_DEFAULT_H__
