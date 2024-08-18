/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetDevice.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/13
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/08/13 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <fstream>
#include <sstream>
#include "SprLog.h"
#include "OneNetDevice.h"

#define SPR_LOGI(fmt, args...) LOGI("OneNetDev", fmt, ##args)
#define SPR_LOGD(fmt, args...) LOGD("OneNetDev", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("OneNetDev", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("OneNetDev", fmt, ##args)

#define ONENET_DEVICE_CFG_PATH "OneNetDevices.conf"

OneNetDevice::OneNetDevice(ModuleIDType id, const std::string& name)
    : SprObserverWithMQueue(id, name), mExpirationTime(0)
{
}

OneNetDevice::~OneNetDevice()
{
}

int32_t OneNetDevice::Init()
{
    DumpDeviceInfomation();
    return VerifyDeviceDetails();
}

int32_t OneNetDevice::DumpDeviceInfomation()
{
    SPR_LOGI("------------------ Dump OneNet Device Information ------------------\n");
    SPR_LOGI("- ExpirationTime  : %d\n", mExpirationTime);
    SPR_LOGI("- OneDevName      : %s\n", mOneDevName.c_str());
    SPR_LOGI("- OneProductID    : %s\n", mOneProductID.c_str());
    SPR_LOGI("- OneKey          : %s\n", mOneKey.c_str());
    SPR_LOGI("- OneToken        : %s\n", mOneToken.c_str());
    SPR_LOGI("--------------------------------------------------------------------\n");
    return 0;
}

int32_t OneNetDevice::VerifyDeviceDetails()
{
    if (mOneDevName.empty()) {
        SPR_LOGE("Device name is empty!\n");
        return -1;
    }
    if (mOneProductID.empty()) {
        SPR_LOGE("Project ID is empty!\n");
        return -1;
    }
    if (mOneKey.empty()) {
        SPR_LOGE("API key is empty!\n");
        return -1;
    }
    if (mOneToken.empty()) {
        SPR_LOGE("API token is empty!\n");
        return -1;
    }
    if (mExpirationTime <= 0) {
        SPR_LOGE("Expiration time is invalid!\n");
        return -1;
    }

    return 0;
}

int32_t OneNetDevice::ProcessMsg(const SprMsg& msg)
{
    return 0;
}
