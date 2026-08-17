/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwInterfaceHub.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/07/29
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "HwInterfaceHub.h"

HwInterfaceHub::PowerCreator    HwInterfaceHub::mPowerCreator    = nullptr;
HwInterfaceHub::GpioCreator     HwInterfaceHub::mGpioCreator     = nullptr;
HwInterfaceHub::UartCreator     HwInterfaceHub::mUartCreator     = nullptr;
HwInterfaceHub::WatchdogCreator HwInterfaceHub::mWatchdogCreator = nullptr;
HwInterfaceHub::RtcCreator      HwInterfaceHub::mRtcCreator      = nullptr;
HwInterfaceHub::NetworkCreator  HwInterfaceHub::mNetworkCreator  = nullptr;
HwInterfaceHub::DeviceCreator   HwInterfaceHub::mDeviceCreator   = nullptr;
HwInterfaceHub::SimCreator      HwInterfaceHub::mSimCreator      = nullptr;

void HwInterfaceHub::RegisterPower(PowerCreator c) {
    mPowerCreator = std::move(c);
}

void HwInterfaceHub::RegisterGpio(GpioCreator c) {
    mGpioCreator = std::move(c);
}

void HwInterfaceHub::RegisterUart(UartCreator c) {
    mUartCreator = std::move(c);
}

void HwInterfaceHub::RegisterWatchdog(WatchdogCreator c) {
    mWatchdogCreator = std::move(c);
}

void HwInterfaceHub::RegisterRtc(RtcCreator c) {
    mRtcCreator = std::move(c);
}

void HwInterfaceHub::RegisterNetwork(NetworkCreator c) {
    mNetworkCreator = std::move(c);
}

void HwInterfaceHub::RegisterDevice(DeviceCreator c) {
    mDeviceCreator = std::move(c);
}

void HwInterfaceHub::RegisterSim(SimCreator c) {
    mSimCreator = std::move(c);
}

void HwInterfaceHub::UnregisterPower() {
    mPowerCreator = nullptr;
}

void HwInterfaceHub::UnregisterGpio() {
    mGpioCreator = nullptr;
}

void HwInterfaceHub::UnregisterUart() {
    mUartCreator = nullptr;
}

void HwInterfaceHub::UnregisterWatchdog() {
    mWatchdogCreator = nullptr;
}

void HwInterfaceHub::UnregisterRtc() {
    mRtcCreator = nullptr;
}

void HwInterfaceHub::UnregisterNetwork() {
    mNetworkCreator = nullptr;
}

void HwInterfaceHub::UnregisterDevice() {
    mDeviceCreator = nullptr;
}

void HwInterfaceHub::UnregisterSim() {
    mSimCreator = nullptr;
}

IHwPower* HwInterfaceHub::GetPower() {
    return mPowerCreator ? mPowerCreator() : nullptr;
}

IHwGpio* HwInterfaceHub::GetGpio() {
    return mGpioCreator ? mGpioCreator() : nullptr;
}

IHwUart* HwInterfaceHub::GetUart() {
    return mUartCreator ? mUartCreator() : nullptr;
}

IHwWatchdog* HwInterfaceHub::GetWatchdog() {
    return mWatchdogCreator ? mWatchdogCreator() : nullptr;
}

IHwRtc* HwInterfaceHub::GetRtc() {
    return mRtcCreator ? mRtcCreator() : nullptr;
}

IHwNetwork* HwInterfaceHub::GetNetwork() {
    return mNetworkCreator ? mNetworkCreator() : nullptr;
}

IHwDevice* HwInterfaceHub::GetDevice() {
    return mDeviceCreator ? mDeviceCreator() : nullptr;
}

IHwSim* HwInterfaceHub::GetSim() {
    return mSimCreator ? mSimCreator() : nullptr;
}
