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

using namespace InternalDefs;

#define SPR_LOGI(fmt, args...) LOGI("OneDev", "[%s] " fmt, mOneDevName.c_str(), ##args)
#define SPR_LOGD(fmt, args...) LOGD("OneDev", "[%s] " fmt, mOneDevName.c_str(), ##args)
#define SPR_LOGW(fmt, args...) LOGW("OneDev", "[%s] " fmt, mOneDevName.c_str(), ##args)
#define SPR_LOGE(fmt, args...) LOGE("OneDev", "[%s] " fmt, mOneDevName.c_str(), ##args)

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
    DumpDeviceInformation();
    return VerifyDeviceDetails();
}

int32_t OneNetDevice::DumpDeviceInformation()
{
    SPR_LOGI("------------------ Dump OneNet Device Information ------------------\n");
    SPR_LOGI("- OneExpTime      : %d\n", mExpirationTime);
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

/**
 * @brief Process SIG_ID_ONENET_MGR_DEVICE_CONNECT
 *
 * @param[in] msg
 * @return none
 */
void OneNetDevice::MsgRespondOneNetMgrDeviceConnect(const SprMsg& msg)
{
    int32_t devNameLen = mOneDevName.length();
    int32_t productIdLen = mOneProductID.length();
    int32_t tokenLen = mOneToken.length();

    std::string  payload = std::to_string(devNameLen)   + mOneDevName
                         + std::to_string(productIdLen) + mOneProductID
                         + std::to_string(tokenLen)     + mOneToken;

    SprMsg conMsg(SIG_ID_ONENET_DRV_MQTT_MSG_CONNECT);
    conMsg.SetString(payload);
    NotifyObserver(MODULE_ONENET_DRIVER, conMsg);
}

int32_t OneNetDevice::ProcessMsg(const SprMsg& msg)
{
    SPR_LOGD("Recv msg: %s\n", GetSigName(msg.GetMsgId()));
    switch (msg.GetMsgId())
    {
        case SIG_ID_ONENET_MGR_DEVICE_CONNECT:
        {
            MsgRespondOneNetMgrDeviceConnect(msg);
            break;
        }

        default:
        {
            break;
        }
    }
    return 0;
}
