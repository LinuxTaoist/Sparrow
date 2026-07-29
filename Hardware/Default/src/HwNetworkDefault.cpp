/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwNetworkDefault.cpp
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
#include "HwNetworkDefault.h"

#define HW_NET_DEFAULT_VERSION  "HW_NET_DEFAULT_R1001"

std::string HwNetworkDefault::GetVersion() const                 { return HW_NET_DEFAULT_VERSION; }
int32_t HwNetworkDefault::GetRegistrationState(EHwNetRegState&)  { return HW_ERR_NOT_IMPL; }
int32_t HwNetworkDefault::GetSignalStrength(SHwSignalStrength&)  { return HW_ERR_NOT_IMPL; }
int32_t HwNetworkDefault::GetCellInfo(SHwCellInfo&)              { return HW_ERR_NOT_IMPL; }
