/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwNetworkDefault.h
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
#ifndef __HW_NETWORK_DEFAULT_H__
#define __HW_NETWORK_DEFAULT_H__

#include <string>
#include "IHwNetwork.h"

class HwNetworkDefault : public IHwNetwork {
public:
    HwNetworkDefault() = default;
    ~HwNetworkDefault() override = default;

    std::string GetVersion() const override;
    int32_t GetRegistrationState(EHwNetRegState& state) override;
    int32_t GetSignalStrength(SHwSignalStrength& sig) override;
    int32_t GetCellInfo(SHwCellInfo& info) override;
};

#endif // __HW_NETWORK_DEFAULT_H__
