/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IHwSim.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SIM (Subscriber Identity Module) HAL interface — pure virtual.
 *  @date       : 2026/08/13
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/08/13 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __IHW_SIM_H__
#define __IHW_SIM_H__

#include <cstdint>
#include <functional>
#include <string>
#include "HwCommonTypes.h"

/** @brief  SIM card status-change callback (hot-plug).
 *  @param  [in] slot  Slot id (HW_SIM_SLOT_1 / HW_SIM_SLOT_2).
 *  @param  [in] info  Latest card state after the change.
 */
using HwSimCardStatusCb = std::function<void(EHwSimSlot slot, const SHwSimCardInfo& info)>;

class IHwSim {
public:
    virtual ~IHwSim() = default;

    /** @brief  Get implementation version string.
     *  @return Version in "HW_SIM_<PLAT>_RXXXX" form.
     */
    virtual std::string GetVersion() const = 0;

    // -------------------------------------------------------------------------
    //  Card identity
    // -------------------------------------------------------------------------

    /** @brief  Get IMSI of the card in the given slot.
     *  @param  [in]  slot  Slot to query.
     *  @param  [out] imsi  IMSI string (ASCII).
     *  @return HW_OK on success; HW_ERR_SIM_QUERY_FAILED / HW_ERR_SIM_NOT_PRESENT.
     */
    virtual int32_t GetImsi(EHwSimSlot slot, std::string& imsi) = 0;

    /** @brief  Get ICCID of the card in the given slot.
     *  @param  [in]  slot   Slot to query.
     *  @param  [out] iccid  ICCID string (ASCII).
     *  @return HW_OK on success; HW_ERR_SIM_QUERY_FAILED / HW_ERR_SIM_NOT_PRESENT.
     */
    virtual int32_t GetIccid(EHwSimSlot slot, std::string& iccid) = 0;

    /** @brief  Get phone number stored on the card.
     *  @param  [in]  slot   Slot to query.
     *  @param  [out] phone  Phone number string (ASCII).
     *  @return HW_OK on success; HW_ERR_SIM_QUERY_FAILED / HW_ERR_SIM_NOT_PRESENT.
     */
    virtual int32_t GetPhoneNum(EHwSimSlot slot, std::string& phone) = 0;

    // -------------------------------------------------------------------------
    //  Card state
    // -------------------------------------------------------------------------

    /** @brief  Query current card state synchronously (real-time detection).
     *  @param  [in]  slot  Slot to query.
     *  @param  [out] info  Card state, app state, retry counters, IMSI/ICCID.
     *  @return HW_OK on success; HW_ERR_SIM_QUERY_FAILED / HW_ERR_SIM_NOT_PRESENT.
     */
    virtual int32_t GetCardInfo(EHwSimSlot slot, SHwSimCardInfo& info) = 0;

    /** @brief  Register SIM card status-change callback (hot-plug detection).
     *  @param  cb  void(EHwSimSlot, const SHwSimCardInfo&).
     *  @return HW_OK on success.
     */
    virtual int32_t RegisterCardStatusCallback(HwSimCardStatusCb cb) = 0;

    // -------------------------------------------------------------------------
    //  Slot control
    // -------------------------------------------------------------------------

    /** @brief  Switch the active physical slot.
     *  @param  phySlot  Target physical slot.
     *  @return HW_OK on success.
     */
    virtual int32_t SwitchSlot(EHwSimSlot phySlot) = 0;

    /** @brief  Power up the SIM in the given slot.
     *  @param  slot  Slot to power up.
     *  @return HW_OK on success.
     */
    virtual int32_t PowerUp(EHwSimSlot slot) = 0;

    /** @brief  Power down the SIM in the given slot.
     *  @param  slot  Slot to power down.
     *  @return HW_OK on success.
     */
    virtual int32_t PowerDown(EHwSimSlot slot) = 0;
};

#endif // __IHW_SIM_H__
