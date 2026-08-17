/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IHwNetwork.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Cellular network HAL interface — pure virtual.
 *  @date       : 2026/07/27
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *  2026/08/12 | 1.0.0.2   | Xiang.D        | All methods pure virtual, full Doxygen
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __IHW_NETWORK_H__
#define __IHW_NETWORK_H__

#include <cstdint>
#include <string>
#include <vector>
#include "HwCommonTypes.h"

using HwPdnStatusCb = std::function<void(int32_t callId, const SHwPdnResult& result)>;

class IHwNetwork {
public:
    virtual ~IHwNetwork() = default;

    // -------------------------------------------------------------------------
    //  Version
    // -------------------------------------------------------------------------

    /** @brief  Get implementation version string.
     *  @return Version in "HW_NET_<PLAT>_RXXXX" form.
     */
    virtual std::string GetVersion() const = 0;

    // -------------------------------------------------------------------------
    //  Registration & signal
    // -------------------------------------------------------------------------

    /** @brief  Query current network registration state.
     *  @param  [out] state  Registration state (home / roaming / searching / denied / unknown).
     *  @return HW_OK on success; HW_ERR_NET_QUERY_FAILED or HW_ERR_NET_NO_SERVICE.
     */
    virtual int32_t GetRegistrationState(EHwNetRegState& state) = 0;

    /** @brief  Query signal strength.
     *  @param  [out] sig  RSSI / RSRP / RSRQ / SINR; unknown fields set to sentinel (99 or max).
     *  @return HW_OK on success; HW_ERR_NET_QUERY_FAILED.
     */
    virtual int32_t GetSignalStrength(SHwSignalStrength& sig) = 0;

    /** @brief  Query serving cell identity.
     *  @param  [out] info  MCC, MNC, LAC/TAC, Cell-Id, RAT.
     *  @return HW_OK on success; HW_ERR_NET_NO_SERVICE if not camped.
     */
    virtual int32_t GetCellInfo(SHwCellInfo& info) = 0;

    // -------------------------------------------------------------------------
    //  Data call (PDN)
    // -------------------------------------------------------------------------

    /** @brief  Initiate a data call (async).
     *  @param  [in] profile  APN profile.
     *  @return HW_OK on async start accepted; HW_ERR_GENERAL on failure.
     */
    virtual int32_t StartDataCall(const SHwApnProfile& profile) = 0;

    /** @brief  Tear down a data call.
     *  @param  callId  PDN call id.
     *  @param  simId   SIM slot id.
     *  @return HW_OK on success.
     */
    virtual int32_t StopDataCall(int32_t callId, int32_t simId) = 0;

    /** @brief  Query data-call status synchronously.
     *  @param  callId  PDN call id.
     *  @param  simId   SIM slot id.
     *  @param  [out] result  IP addresses, DNS, gateway, interface name.
     *  @return HW_OK on success; HW_ERR_NET_QUERY_FAILED.
     */
    virtual int32_t QueryDataCall(int32_t callId, int32_t simId, SHwPdnResult& result) = 0;

    // -------------------------------------------------------------------------
    //  Route / DNS
    // -------------------------------------------------------------------------

    /** @brief  Check whether a network interface exists and carrier is up.
     *  @param  [in]  ifaceName  Interface name (e.g. "rmnet0").
     *  @param  [out] isUp       true if the interface exists and is up (LOWER_UP);
     *                           false if the interface does not exist or is down.
     *  @return HW_OK on success; HW_ERR_NET_QUERY_FAILED on query failure.
     */
    virtual int32_t IsInterfaceUp(const std::string& ifaceName, bool& isUp) = 0;

    /** @brief  Get IP addresses assigned to a network interface.
     *  @param  [in]  ifaceName  Interface name (e.g. "rmnet0").
     *  @param  [out] addrs      Assigned IPv4/IPv6 address strings (without prefix).
     *  @return HW_OK on success; HW_ERR_NET_QUERY_FAILED on query failure.
     */
    virtual int32_t GetInterfaceAddrs(const std::string& ifaceName,
                                      std::vector<std::string>& addrs) = 0;

    /** @brief  Set default route via cellular interface.
     *  @param  ifaceName  Data-call interface name (e.g. "rmnet0").
     *  @param  gateway    Gateway IP; empty for link-scope default.
     *  @return HW_OK on success.
     */
    virtual int32_t SetDefaultRoute(const std::string& ifaceName, const std::string& gateway) = 0;

    /** @brief  Remove default route via cellular interface.
     *  @param  ifaceName  Data-call interface name.
     *  @param  gateway    Gateway IP.
     *  @return HW_OK on success.
     */
    virtual int32_t ClearDefaultRoute(const std::string& ifaceName, const std::string& gateway) = 0;

    /** @brief  Add a specific route entry.
     *  @param  route  Destination / gateway / iface / metric.
     *  @return HW_OK on success.
     */
    virtual int32_t AddRoute(const SHwRouteItem& route) = 0;

    /** @brief  Update resolver configuration.
     *  @param  dns1  Primary DNS server.
     *  @param  dns2  Secondary DNS server.
     *  @return HW_OK on success.
     */
    virtual int32_t UpdateDns(const std::string& dns1, const std::string& dns2) = 0;

    /** @brief  Register PDN status-change callback.
     *  @param  cb       void(int32_t callId, const SHwPdnResult&).
     */
    virtual void RegisterPdnStatusCallback(HwPdnStatusCb cb) = 0;

    // -------------------------------------------------------------------------
    //  Route query (diagnose)
    // -------------------------------------------------------------------------

    /** @brief  Check whether a default route via the given interface exists.
     *  @param  [in]  ifaceName  Interface name (e.g. "rmnet0").
     *  @param  [out] isSet      true if a default route via this iface exists.
     *  @return HW_OK on success; HW_ERR_NET_QUERY_FAILED on query failure.
     */
    virtual int32_t IsDefaultRouteSet(const std::string& ifaceName, bool& isSet) = 0;

    /** @brief  Check whether a dedicated route table is non-empty.
     *  @param  [in]  tableId   Dedicated route table id (e.g. 100).
     *  @param  [out] hasRoute  true if the table has at least one route entry.
     *  @return HW_OK on success; HW_ERR_NET_QUERY_FAILED on query failure.
     */
    virtual int32_t HasRouteTableEntry(int32_t tableId, bool& hasRoute) = 0;

    /** @brief  Check whether at least one policy rule looks up the table.
     *  @param  [in]  tableId   Dedicated route table id.
     *  @param  [out] hasRule   true if a rule targets this table.
     *  @return HW_OK on success; HW_ERR_NET_QUERY_FAILED on query failure.
     */
    virtual int32_t HasPolicyRule(int32_t tableId, bool& hasRule) = 0;

    // -------------------------------------------------------------------------
    //  Connectivity probe
    // -------------------------------------------------------------------------

    /** @brief  Resolve a host name to verify DNS works.
     *  @param  [in]  host      Host name to resolve (e.g. "baidu.com").
     *  @param  [out] resolved  true if the host resolves to at least one address.
     *  @return HW_OK on success; HW_ERR_NET_QUERY_FAILED on query failure.
     */
    virtual int32_t ResolveHost(const std::string& host, bool& resolved) = 0;

    /** @brief  Ping a host to verify end-to-end connectivity.
     *  @param  [in]  host       Host name or IP (e.g. "223.5.5.5").
     *  @param  [in]  timeoutMs  Timeout in milliseconds for the probe.
     *  @param  [out] reachable  true if the host replies.
     *  @return HW_OK on success; HW_ERR_NET_QUERY_FAILED on query failure.
     */
    virtual int32_t PingHost(const std::string& host, int32_t timeoutMs, bool& reachable) = 0;

    // -------------------------------------------------------------------------
    //  Policy routing (private-network PDN)
    // -------------------------------------------------------------------------

    /** @brief  Establish policy routing for a private-network PDN:
     *           dedicated route table + fwmark based rules.
     *  @param  tableId   Dedicated route table id (e.g. 100).
     *  @param  ifaceName Private PDN interface name.
     *  @param  gateway   Private PDN gateway; empty for link-scope.
     *  @param  fwMarks   fwmark values (low 16 bits) to steer into tableId.
     *  @return HW_OK on success.
     */
    virtual int32_t SetPolicyRoute(int32_t tableId, const std::string& ifaceName,
                                   const std::string& gateway,
                                   const std::vector<uint32_t>& fwMarks) = 0;

    /** @brief  Tear down policy routing for a private-network PDN.
     *  @param  tableId   Dedicated route table id.
     *  @param  ifaceName Private PDN interface name.
     *  @return HW_OK on success.
     */
    virtual int32_t ClearPolicyRoute(int32_t tableId, const std::string& ifaceName) = 0;
};

#endif // __IHW_NETWORK_H__
