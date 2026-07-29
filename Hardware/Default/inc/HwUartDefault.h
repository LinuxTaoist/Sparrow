/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwUartDefault.h
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
#ifndef __HW_UART_DEFAULT_H__
#define __HW_UART_DEFAULT_H__

#include <string>
#include "IHwUart.h"

class HwUartDefault : public IHwUart {
public:
    HwUartDefault() = default;
    ~HwUartDefault() override = default;

    std::string GetVersion() const override;
    int32_t Open(const std::string& port, const SHwUartConfig& config) override;
    int32_t Close() override;
    int32_t Send(const uint8_t* data, int32_t len) override;
    int32_t RegisterRecvHandler(HwUartRecvHandler handler) override;
    int32_t Flush() override;
    bool IsOpen() const override;
};

#endif // __HW_UART_DEFAULT_H__
