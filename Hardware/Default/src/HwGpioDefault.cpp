/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwGpioDefault.cpp
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
#include "HwGpioDefault.h"

#define HW_GPIO_DEFAULT_VERSION  "HW_GPIO_DEFAULT_R1001"

std::string HwGpioDefault::GetVersion() const                           { return HW_GPIO_DEFAULT_VERSION; }
int32_t HwGpioDefault::Init(const std::vector<SHwGpioPinConfig>&)       { return HW_ERR_NOT_IMPL; }
int32_t HwGpioDefault::Release()                                        { return HW_ERR_NOT_IMPL; }
int32_t HwGpioDefault::Read(int32_t, EHwGpioValue&)                     { return HW_ERR_NOT_IMPL; }
int32_t HwGpioDefault::Write(int32_t, EHwGpioValue)                     { return HW_ERR_NOT_IMPL; }
int32_t HwGpioDefault::RegisterIrqHandler(int32_t, HwGpioIrqHandler)    { return HW_ERR_NOT_IMPL; }
int32_t HwGpioDefault::EnableIrq(int32_t)                               { return HW_ERR_NOT_IMPL; }
int32_t HwGpioDefault::DisableIrq(int32_t)                              { return HW_ERR_NOT_IMPL; }
