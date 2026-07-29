/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwRtcDefault.h
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
#ifndef __HW_RTC_DEFAULT_H__
#define __HW_RTC_DEFAULT_H__

#include <string>
#include "IHwRtc.h"

class HwRtcDefault : public IHwRtc {
public:
    HwRtcDefault() = default;
    ~HwRtcDefault() override = default;

    std::string GetVersion() const override;
    int32_t GetTime(int64_t& timestampSec) override;
    int32_t SetTime(int64_t timestampSec) override;
    int32_t SetAlarm(int64_t timestampSec, HwRtcAlarmHandler handler) override;
    int32_t CancelAlarm() override;
};

#endif // __HW_RTC_DEFAULT_H__
