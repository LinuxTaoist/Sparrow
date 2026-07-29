/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IHwNetwork.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/07/27
 *
 *  Cellular network HAL interface.
 *
 *  This interface abstracts the cellular modem's network status queries.
 *  Implementations typically bridge to QMI, AT commands, or vendor ioctls.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __IHW_NETWORK_H__
#define __IHW_NETWORK_H__

#include <cstdint>
#include <string>
#include "HwCommonTypes.h"

class IHwNetwork {
public:
    virtual ~IHwNetwork() = default;

    virtual std::string GetVersion() const = 0;

    /**
     * @brief Query the current network registration state.
     *
     * @param  [out] state  registration state
     * @return HW_OK on success, HW_ERR_NET_QUERY_FAILED
     */
    virtual int32_t GetRegistrationState(EHwNetRegState& state) = 0;

    /**
     * @brief Query signal strength (RSSI for GSM/UMTS; RSRP/RSRQ/SINR for LTE).
     *
     * Fields set to 99 / maximum indicate "unknown" as per 3GPP.
     *
     * @param  [out] sig  signal strength metrics
     * @return HW_OK on success, HW_ERR_NET_QUERY_FAILED
     */
    virtual int32_t GetSignalStrength(SHwSignalStrength& sig) = 0;

    /**
     * @brief Query serving cell information (MCC, MNC, LAC/TAC, Cell ID, RAT).
     *
     * The CellInfo struct is caller-allocated; the implementation fills it.
     * The RAT string points to static or constant memory (do NOT free).
     *
     * @param  [out] info  serving cell info
     * @return HW_OK on success, HW_ERR_NET_NO_SERVICE if not camped
     */
    virtual int32_t GetCellInfo(SHwCellInfo& info) = 0;
};

#endif // __IHW_NETWORK_H__
