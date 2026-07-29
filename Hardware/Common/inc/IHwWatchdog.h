/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IHwWatchdog.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/07/27
 *
 *  Hardware watchdog HAL interface.
 *
 *  Typical call sequence:
 *    1. Start(timeoutSec)       — enable with a timeout
 *    2. Feed()                   — kick periodically (caller's timer / epoll)
 *    3. Stop()                   — disable before shutdown
 *
 *  The watchdog is a safety net: if Feed() is not called within
 *  timeoutSec seconds, the system resets.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __IHW_WATCHDOG_H__
#define __IHW_WATCHDOG_H__

#include <cstdint>
#include <string>
#include "HwCommonTypes.h"

class IHwWatchdog {
public:
    virtual ~IHwWatchdog() = default;

    virtual std::string GetVersion() const = 0;

    /**
     * @brief Start the hardware watchdog.
     *
     * After this call, the application MUST call Feed() periodically at
     * an interval shorter than timeoutSec, or the system will reboot.
     *
     * @param  timeoutSec  watchdog timeout in seconds (typical: 10 ~ 60)
     * @return HW_OK on success, HW_ERR_WDT_OPEN_FAILED
     */
    virtual int32_t Start(int32_t timeoutSec) = 0;

    /**
     * @brief Kick (feed) the watchdog to prevent a reset.
     *
     * Call this from a periodic timer at interval < timeoutSec / 2.
     *
     * @return HW_OK on success, HW_ERR_WDT_FEED_FAILED
     */
    virtual int32_t Feed() = 0;

    /**
     * @brief Stop (disable) the watchdog.
     *
     * Some hardware watchdogs cannot be stopped once started (design
     * constraint); in that case return HW_ERR_NOT_IMPL.
     *
     * @return HW_OK on success, HW_ERR_NOT_IMPL if stopping is not supported
     */
    virtual int32_t Stop() = 0;
};

#endif // __IHW_WATCHDOG_H__
