/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwUartDefault.cpp
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
#include <string>
#include "HwCommonTypes.h"
#include "HwUartDefault.h"

#define HW_UART_DEFAULT_VERSION  "HW_UART_DEFAULT_R1001"

std::string HwUartDefault::GetVersion() const                           { return HW_UART_DEFAULT_VERSION; }
int32_t HwUartDefault::Open(const std::string&, const SHwUartConfig&)   { return HW_ERR_NOT_IMPL; }
int32_t HwUartDefault::Close()                                          { return HW_ERR_NOT_IMPL; }
int32_t HwUartDefault::Send(const uint8_t*, int32_t)                    { return HW_ERR_NOT_IMPL; }
int32_t HwUartDefault::RegisterRecvHandler(HwUartRecvHandler)           { return HW_ERR_NOT_IMPL; }
int32_t HwUartDefault::Flush()                                          { return HW_ERR_NOT_IMPL; }
bool    HwUartDefault::IsOpen() const                                   { return false; }
