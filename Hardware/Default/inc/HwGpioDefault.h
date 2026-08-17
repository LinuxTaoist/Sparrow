/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwGpioDefault.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/07/27
 *
 *  Default (stub) implementation of IHwGpio.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __HW_GPIO_DEFAULT_H__
#define __HW_GPIO_DEFAULT_H__

#include <string>
#include <vector>
#include "IHwGpio.h"

class HwGpioDefault : public IHwGpio {
public:
    HwGpioDefault() = default;
    ~HwGpioDefault() override = default;

    std::string GetVersion() const override;
    int32_t Init(const std::vector<SHwGpioPinConfig>& cfgs) override;
    int32_t Release() override;
    int32_t Read(int32_t pin, EHwGpioValue& value) override;
    int32_t Write(int32_t pin, EHwGpioValue value) override;
    int32_t RegisterIrqCallback(int32_t pin, HwGpioIrqCb handler) override;
    int32_t EnableIrq(int32_t pin) override;
    int32_t DisableIrq(int32_t pin) override;
};

#endif // __HW_GPIO_DEFAULT_H__
