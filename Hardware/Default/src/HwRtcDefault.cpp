/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwRtcDefault.cpp
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
#include "HwRtcDefault.h"

#define HW_RTC_DEFAULT_VERSION  "HW_RTC_DEFAULT_R1001"

std::string HwRtcDefault::GetVersion() const                    { return HW_RTC_DEFAULT_VERSION; }
int32_t HwRtcDefault::GetTime(int64_t&)                         { return HW_ERR_NOT_IMPL; }
int32_t HwRtcDefault::SetTime(int64_t)                          { return HW_ERR_NOT_IMPL; }
int32_t HwRtcDefault::SetAlarm(int64_t, HwRtcAlarmHandler)      { return HW_ERR_NOT_IMPL; }
int32_t HwRtcDefault::CancelAlarm()                             { return HW_ERR_NOT_IMPL; }
