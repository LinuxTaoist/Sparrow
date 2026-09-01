/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IHwGpio.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/07/27
 *
 *  GPIO HAL interface.
 *
 *  Typical call sequence:
 *    1. Init(pinCfgs, count)              — export + configure all pins
 *    2. Read() / Write()                  — polling-style access
 *    3. RegisterIrqCallback()              — set up interrupt on edge
 *    4. EnableIrq() → handler fires       — interrupt-driven
 *    5. DisableIrq() / Release()          — tear down
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __IHW_GPIO_H__
#define __IHW_GPIO_H__

#include <cstdint>
#include <string>
#include <vector>
#include "HwCommonTypes.h"

using HwGpioIrqCb = std::function<void(int32_t pin)>;

class IHwGpio {
public:
    virtual ~IHwGpio() = default;

    virtual std::string GetVersion() const = 0;

    /**
     * @brief Export and configure a batch of GPIO pins.
     *
     * This is the mandatory first call.  It exports each pin, sets
     * direction / pull / edge, and validates that the pins exist.
     *
     * @param  cfgs    array of pin configurations
     * @param  count   number of elements in cfgs
     * @return HW_OK on success, or HW_ERR_GPIO_EXPORT_FAILED / HW_ERR_GPIO_DIRECTION_FAILED
     */
    virtual int32_t Init(const std::vector<SHwGpioPinConfig>& cfgs) = 0;

    /**
     * @brief Unexport all pins and release resources.
     * @return HW_OK on success
     */
    virtual int32_t Release() = 0;

    /**
     * @brief Read the logical level of a pin.
     *
     * The pin MUST have been previously configured via Init().
     *
     * @param  pin    pin number
     * @param  [out] value  current level
     * @return HW_OK on success, HW_ERR_GPIO_READ_FAILED / HW_ERR_GPIO_INVALID_PIN
     */
    virtual int32_t Read(int32_t pin, EHwGpioValue& value) = 0;

    /**
     * @brief Write a logical level to a pin.
     *
     * The pin MUST have been configured as OUTPUT via Init().
     *
     * @param  pin    pin number
     * @param  value  target level
     * @return HW_OK on success, HW_ERR_GPIO_WRITE_FAILED / HW_ERR_GPIO_INVALID_PIN
     */
    virtual int32_t Write(int32_t pin, EHwGpioValue value) = 0;

    /**
     * @brief Register an interrupt callback for a specific pin.
     *
     * Must be called before EnableIrq().
     *
     * @param  pin      pin number
     * @param  handler  callback object
     * @return HW_OK on success
     */
    virtual int32_t RegisterIrqCallback(int32_t pin, HwGpioIrqCb cb) = 0;

    /**
     * @brief Enable interrupt on a specific pin.
     *
     * The pin MUST have been configured with a non-NONE edge in Init().
     *
     * @param  pin   pin number
     * @return HW_OK on success, HW_ERR_GPIO_IRQ_FAILED if not available
     */
    virtual int32_t EnableIrq(int32_t pin) = 0;

    /**
     * @brief Disable interrupt on a specific pin.
     * @param  pin   pin number
     * @return HW_OK on success
     */
    virtual int32_t DisableIrq(int32_t pin) = 0;
};

#endif // __IHW_GPIO_H__
