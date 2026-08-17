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

std::string HwNetworkDefault::GetVersion() const {
    return HW_NET_DEFAULT_VERSION;
}

int32_t HwNetworkDefault::GetRegistrationState(EHwNetRegState&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::GetSignalStrength(SHwSignalStrength&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::GetCellInfo(SHwCellInfo&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::StartDataCall(const SHwApnProfile&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::StopDataCall(int32_t, int32_t) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::QueryDataCall(int32_t, int32_t, SHwPdnResult&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::SetDefaultRoute(const std::string&, const std::string&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::ClearDefaultRoute(const std::string&, const std::string&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::AddRoute(const SHwRouteItem&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::IsInterfaceUp(const std::string&, bool&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::GetInterfaceAddrs(const std::string&, std::vector<std::string>&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::IsDefaultRouteSet(const std::string&, bool&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::HasRouteTableEntry(int32_t, bool&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::HasPolicyRule(int32_t, bool&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::ResolveHost(const std::string&, bool&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::PingHost(const std::string&, int32_t, bool&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::UpdateDns(const std::string& dns1, const std::string& dns2) {
    return HW_ERR_NOT_IMPL;
}

void HwNetworkDefault::RegisterPdnStatusCallback(HwPdnStatusCb) {
}

int32_t HwNetworkDefault::SetPolicyRoute(int32_t, const std::string&, const std::string&,
                                         const std::vector<uint32_t>&) {
    return HW_ERR_NOT_IMPL;
}

int32_t HwNetworkDefault::ClearPolicyRoute(int32_t, const std::string&) {
    return HW_ERR_NOT_IMPL;
}
