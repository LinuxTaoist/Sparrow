/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IHwDevice.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Device management HAL interface — pure virtual.
 *  @date       : 2026/08/12
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/08/12 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __IHW_DEVICE_H__
#define __IHW_DEVICE_H__

#include <cstdint>
#include <functional>
#include <string>
#include "HwCommonTypes.h"

using HwAirPlaneModeCb = std::function<void(bool on)>;

class IHwDevice {
public:
    virtual ~IHwDevice() = default;

    /** @brief  Get implementation version string.
     *  @return Version in "HW_DEV_<PLAT>_RXXXX" form.
     */
    virtual std::string GetVersion() const = 0;

    /** @brief  Set modem radio power (≈ AT+CFUN).
     *  @param  on  true = radio on (CFUN=1), false = radio off (CFUN=0).
     *  @return HW_OK on success.
     */
    virtual int32_t SetRadio(bool on) = 0;

    /** @brief  Set airplane mode.
     *  @param  on  true = enter airplane mode, false = exit.
     *  @return HW_OK on success.
     */
    virtual int32_t SetAirplaneMode(bool on) = 0;

    /** @brief  Query current airplane mode.
     *  @param  [out] on  true = airplane mode on.
     *  @return HW_OK on success.
     */
    virtual int32_t GetAirplaneMode(EHwAirplaneMode& mode) = 0;

    /** @brief  Query modem operational state.
     *  @param  [out] state  Online / offline / unknown.
     *  @return HW_OK on success.
     */
    virtual int32_t GetModemState(EHwModemState& state) = 0;

    /** @brief  Register airplane mode change callback.
     *  @param  cb   void(bool on) — true = airplane mode entered.
     *  @return HW_OK on success.
     */
    virtual int32_t RegisterAirPlaneModeCallback(HwAirPlaneModeCb cb) = 0;
};

#endif // __IHW_DEVICE_H__