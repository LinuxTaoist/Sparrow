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
 *  UART factories create an independently owned instance per call. Other
 *  device factories return a function-local static instance.
 *---------------------------------------------------------------------------------------------------------------------
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
            []() -> std::unique_ptr<IHwUart> {
                return std::unique_ptr<IHwUart>(new HwUartDefault());
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
