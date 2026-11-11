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
 *  Registration happens once before main() via a static HwRegister* global.
 *  GetXxx() returns a platform singleton, except for UART: CreateUart() creates
 *  an independently owned instance for one port. Its owner controls the port,
 *  receive callback and lifetime without affecting other UART instances.
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __HW_INTERFACE_HUB_H__
#define __HW_INTERFACE_HUB_H__

#include <memory>
#include <functional>

class IHwPower;
class IHwGpio;
class IHwUart;
class IHwWatchdog;
class IHwRtc;
class IHwNetwork;
class IHwDevice;
class IHwSim;

class HwInterfaceHub {
public:
    HwInterfaceHub() = delete;
    ~HwInterfaceHub() = delete;
    HwInterfaceHub(const HwInterfaceHub&) = delete;
    HwInterfaceHub& operator=(const HwInterfaceHub&) = delete;

    using PowerCreator    = std::function<IHwPower*()>;
    using GpioCreator     = std::function<IHwGpio*()>;
    using WatchdogCreator = std::function<IHwWatchdog*()>;
    using RtcCreator      = std::function<IHwRtc*()>;
    using NetworkCreator  = std::function<IHwNetwork*()>;
    using DeviceCreator   = std::function<IHwDevice*()>;
    using SimCreator      = std::function<IHwSim*()>;
    using UartCreator     = std::function<std::unique_ptr<IHwUart>()>;

    static void RegisterPower(PowerCreator c);
    static void RegisterGpio(GpioCreator c);
    static void RegisterWatchdog(WatchdogCreator c);
    static void RegisterRtc(RtcCreator c);
    static void RegisterNetwork(NetworkCreator c);
    static void RegisterDevice(DeviceCreator c);
    static void RegisterSim(SimCreator c);
    static void RegisterUart(UartCreator c);

    static void UnregisterPower();
    static void UnregisterGpio();
    static void UnregisterWatchdog();
    static void UnregisterRtc();
    static void UnregisterNetwork();
    static void UnregisterDevice();
    static void UnregisterSim();
    static void UnregisterUart();

    static IHwPower*     GetPower();
    static IHwGpio*      GetGpio();
    static IHwWatchdog*  GetWatchdog();
    static IHwRtc*       GetRtc();
    static IHwNetwork*   GetNetwork();
    static IHwDevice*    GetDevice();
    static IHwSim*       GetSim();
    static std::unique_ptr<IHwUart> CreateUart();

private:
    static PowerCreator    mPowerCreator;
    static GpioCreator     mGpioCreator;
    static WatchdogCreator mWatchdogCreator;
    static RtcCreator      mRtcCreator;
    static NetworkCreator  mNetworkCreator;
    static DeviceCreator   mDeviceCreator;
    static SimCreator      mSimCreator;
    static UartCreator     mUartCreator;
};

#endif // __HW_INTERFACE_HUB_H__
