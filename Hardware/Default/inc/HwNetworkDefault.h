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
    int32_t StartDataCall(const SHwApnProfile& profile) override;
    int32_t StopDataCall(int32_t callId, int32_t simId) override;
    int32_t QueryDataCall(int32_t callId, int32_t simId, SHwPdnResult& result) override;
    int32_t SetDefaultRoute(const std::string& ifaceName, const std::string& gateway) override;
    int32_t ClearDefaultRoute(const std::string& ifaceName, const std::string& gateway) override;
    int32_t AddRoute(const SHwRouteItem& route) override;
    int32_t IsInterfaceUp(const std::string& ifaceName, bool& isUp) override;
    int32_t GetInterfaceAddrs(const std::string& ifaceName,
                              std::vector<std::string>& addrs) override;
    int32_t IsDefaultRouteSet(const std::string& ifaceName, bool& isSet) override;
    int32_t HasRouteTableEntry(int32_t tableId, bool& hasRoute) override;
    int32_t HasPolicyRule(int32_t tableId, bool& hasRule) override;
    int32_t ResolveHost(const std::string& host, bool& resolved) override;
    int32_t PingHost(const std::string& host, int32_t timeoutMs, bool& reachable) override;
    int32_t UpdateDns(const std::string& dns1, const std::string& dns2) override;
    void    RegisterPdnStatusCallback(HwPdnStatusCb handler) override;
    int32_t SetPolicyRoute(int32_t tableId, const std::string& ifaceName,
                           const std::string& gateway,
                           const std::vector<uint32_t>& fwMarks) override;
    int32_t ClearPolicyRoute(int32_t tableId, const std::string& ifaceName) override;
};

#endif // __HW_NETWORK_DEFAULT_H__
