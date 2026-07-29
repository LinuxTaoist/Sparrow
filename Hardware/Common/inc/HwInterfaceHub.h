/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwInterfaceHub.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/07/29
 *
 *  Central registry for HAL implementations (pure static class).
 *
 *  All HAL devices are SINGLETON — one instance per device type manages
 *  all underlying hardware resources exclusively, avoiding resource contention.
 *
 *  Registration happens once before main() via a static HwRegister* global.
 *  After that, business code calls GetXxx() to obtain a reference to the
 *  singleton instance.  C++11 guarantees thread-safe initialisation of
 *  function-local statics, so no explicit locking is needed.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __HW_INTERFACE_HUB_H__
#define __HW_INTERFACE_HUB_H__

#include <functional>

class IHwPower;
class IHwGpio;
class IHwUart;
class IHwWatchdog;
class IHwRtc;
class IHwNetwork;

class HwInterfaceHub {
public:
    HwInterfaceHub() = delete;
    ~HwInterfaceHub() = delete;
    HwInterfaceHub(const HwInterfaceHub&) = delete;
    HwInterfaceHub& operator=(const HwInterfaceHub&) = delete;

    using PowerCreator    = std::function<IHwPower&()>;
    using GpioCreator     = std::function<IHwGpio&()>;
    using UartCreator     = std::function<IHwUart&()>;
    using WatchdogCreator = std::function<IHwWatchdog&()>;
    using RtcCreator      = std::function<IHwRtc&()>;
    using NetworkCreator  = std::function<IHwNetwork&()>;

    static void RegisterPower(PowerCreator c);
    static void RegisterGpio(GpioCreator c);
    static void RegisterUart(UartCreator c);
    static void RegisterWatchdog(WatchdogCreator c);
    static void RegisterRtc(RtcCreator c);
    static void RegisterNetwork(NetworkCreator c);

    static void UnregisterPower();
    static void UnregisterGpio();
    static void UnregisterUart();
    static void UnregisterWatchdog();
    static void UnregisterRtc();
    static void UnregisterNetwork();

    static IHwPower*     GetPower();
    static IHwGpio*      GetGpio();
    static IHwUart*      GetUart();
    static IHwWatchdog*  GetWatchdog();
    static IHwRtc*       GetRtc();
    static IHwNetwork*   GetNetwork();

private:
    static PowerCreator    mPowerCreator;
    static GpioCreator     mGpioCreator;
    static UartCreator     mUartCreator;
    static WatchdogCreator mWatchdogCreator;
    static RtcCreator      mRtcCreator;
    static NetworkCreator  mNetworkCreator;
};

#endif // __HW_INTERFACE_HUB_H__
