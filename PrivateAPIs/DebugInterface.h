/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DebugInterface.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/28
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/28 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __DEBUG_INTERFACE_H__
#define __DEBUG_INTERFACE_H__

#include <stdint.h>

class DebugInterface
{
public:
    ~DebugInterface();
    static DebugInterface* GetInstance();

    int AddTimerInOneSec();
    int DelTimerInOneSec();
    int AddCustomTimer(uint32_t RepeatTimes, int32_t DelayInMilliSec, int32_t IntervalInMilliSec);
    int DelCustomTimer();
    int EnableRemoteShell();
    int DisableRemoteShell();

private:
    DebugInterface();

private:
    bool mEnable;
};

#endif // __DEBUG_INTERFACE_H__
