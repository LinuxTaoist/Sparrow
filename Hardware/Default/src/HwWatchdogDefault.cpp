/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwWatchdogDefault.cpp
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
#include "HwWatchdogDefault.h"

#define HW_WDT_DEFAULT_VERSION  "HW_WDT_DEFAULT_R1001"

std::string HwWatchdogDefault::GetVersion() const  { return HW_WDT_DEFAULT_VERSION; }
int32_t HwWatchdogDefault::Start(int32_t)          { return HW_ERR_NOT_IMPL; }
int32_t HwWatchdogDefault::Feed()                  { return HW_ERR_NOT_IMPL; }
int32_t HwWatchdogDefault::Stop()                  { return HW_ERR_NOT_IMPL; }
