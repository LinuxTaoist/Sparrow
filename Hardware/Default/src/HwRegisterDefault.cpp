/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwRegisterDefault.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/07/29
 *
 *  Auto-registers all Default (stub) HAL implementations with HwInterfaceHub.
 *  The static global object guarantees registration before main().
 *
 *  Real platforms provide their own HwRegister*.cpp in Hardware/<Family>/.
 *  They must follow the same pattern: each factory uses a function-local
 *  static instance and returns a reference.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "HwInterfaceHub.h"
#include "HwPowerDefault.h"
#include "HwGpioDefault.h"
#include "HwUartDefault.h"
#include "HwWatchdogDefault.h"
#include "HwRtcDefault.h"
#include "HwNetworkDefault.h"
#include "HwDeviceDefault.h"
#include "HwSimDefault.h"

namespace {

struct HwRegisterDefault {
    HwRegisterDefault() {
        HwInterfaceHub::RegisterPower(
            []() -> IHwPower* {
                static HwPowerDefault instance;
                return &instance;
            });

        HwInterfaceHub::RegisterGpio(
            []() -> IHwGpio* {
                static HwGpioDefault instance;
                return &instance;
            });

        HwInterfaceHub::RegisterUart(
            []() -> IHwUart* {
                static HwUartDefault instance;
                return &instance;
            });

        HwInterfaceHub::RegisterWatchdog(
            []() -> IHwWatchdog* {
                static HwWatchdogDefault instance;
                return &instance;
            });

        HwInterfaceHub::RegisterRtc(
            []() -> IHwRtc* {
                static HwRtcDefault instance;
                return &instance;
            });

        HwInterfaceHub::RegisterNetwork(
            []() -> IHwNetwork* {
                static HwNetworkDefault instance;
                return &instance;
            });

        HwInterfaceHub::RegisterDevice(
            []() -> IHwDevice* {
                static HwDeviceDefault instance;
                return &instance;
            });

        HwInterfaceHub::RegisterSim(
            []() -> IHwSim* {
                static HwSimDefault instance;
                return &instance;
            });
    }
} HwRegisterDefault;

} // anonymous namespace
