/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IHwRtc.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/07/27
 *
 *  RTC HAL interface.
 *
 *  Typical call sequence:
 *    1. GetTime() / SetTime()            — sync system ↔ hardware clock
 *    2. SetAlarm(ts, handler)            — schedule wake-up
 *    3. CancelAlarm()                    — clear
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __IHW_RTC_H__
#define __IHW_RTC_H__

#include <cstdint>
#include <string>
#include "HwCommonTypes.h"

class IHwRtc {
public:
    virtual ~IHwRtc() = default;

    virtual std::string GetVersion() const = 0;

    /**
     * @brief Read the hardware RTC as a Unix timestamp (seconds since epoch, UTC).
     *
     * @param  [out] timestampSec  current RTC time (seconds since epoch, UTC)
     * @return HW_OK on success, HW_ERR_RTC_READ_FAILED
     */
    virtual int32_t GetTime(int64_t& timestampSec) = 0;

    /**
     * @brief Write a Unix timestamp to the hardware RTC.
     *
     * @param  timestampSec  time to set (seconds since epoch, UTC)
     * @return HW_OK on success, HW_ERR_RTC_SET_FAILED
     */
    virtual int32_t SetTime(int64_t timestampSec) = 0;

    /**
     * @brief Schedule a wake-up alarm.
     *
     * When the RTC reaches timestampSec, the platform wakes from sleep
     * and invokes the registered callback, if any.
     *
     * Only ONE alarm may be active at a time; a second call replaces
     * the previous alarm.
     *
     * @param  timestampSec  wake-up time (seconds since epoch, UTC)
     * @param  handler       callback object; may be empty
     * @return HW_OK on success
     */
    virtual int32_t SetAlarm(int64_t timestampSec, HwRtcAlarmHandler handler) = 0;

    /**
     * @brief Cancel any pending alarm.
     * @return HW_OK on success
     */
    virtual int32_t CancelAlarm() = 0;
};

#endif // __IHW_RTC_H__
