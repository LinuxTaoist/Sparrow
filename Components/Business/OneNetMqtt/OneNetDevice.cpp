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

#define ONENET_DEVICE_CFG_PATH          "OneNetDevices.conf"
#define DEFAULT_KEEP_ALIVE_INTERVAL     60  // 60s

#define TEMPLATE_TOPIC_THING_ALL            "$sys/%s/%s/thing/%s"
#define TEMPLATE_TOPIC_THING_POST_REPLY     "$sys/%s/%s/thing/property/%s"
#define TEMPLATE_TOPIC_CMD                  "$sys/%s/%s/cmd/%s"

OneNetDevice::OneNetDevice(ModuleIDType id, const std::string& name)
    : SprObserverWithMQueue(id, name), mExpirationTime(0), mConnectStatus(false)
{
    mCurSubscribeIdx = 0;
    mKeepAliveIntervalInSec = DEFAULT_KEEP_ALIVE_INTERVAL;
}

OneNetDevice::~OneNetDevice()
{
}

int32_t OneNetDevice::Init()
{
    DumpDeviceInformation();
    VerifyDeviceDetails();
    InitTopicList();
    return 0;
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

int32_t OneNetDevice::InitTopicList()
{
    char topicThingAll[128] = {0};
    snprintf(topicThingAll, sizeof(topicThingAll), TEMPLATE_TOPIC_THING_POST_REPLY, mOneProductID.c_str(), mOneDevName.c_str(), "post/reply");
    mAllTopics.push_back(topicThingAll);
    SPR_LOGI("Add topic: %s\n", topicThingAll);

    // char topicCmd[128] = {0};
    // snprintf(topicCmd, sizeof(topicCmd), TEMPLATE_TOPIC_CMD, mOneProductID.c_str(), mOneDevName.c_str(), "#");
    // mAllTopics.push_back(topicCmd);
    return 0;
}

int16_t OneNetDevice::GetUnusedIdentity()
{
    int16_t identity = 1;
    if (mUsedIdentities.empty()) {
        mUsedIdentities.insert(identity);
        return identity;
    }

    if (*(--mUsedIdentities.end()) == (uint16_t)mUsedIdentities.size()) {
        identity = (uint16_t)(mUsedIdentities.size() + 1);
        mUsedIdentities.insert(identity);
        return identity;
    }

    for (uint16_t i = 1; i < (uint16_t)mUsedIdentities.size(); i++) {
        if (mUsedIdentities.find(i) == mUsedIdentities.end()) {
            identity = i;
            mUsedIdentities.insert(identity);
            return identity;
        }
    }

    return identity;
}

void OneNetDevice::StartSubscribeTopic()
{
    mCurSubscribeIdx = 0;
    SprMsg msg(SIG_ID_ONENET_DEV_SUBSCRIBE_TOPIC);
    SendMsg(msg);
}

/**
 * @brief Process SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT
 *
 * @param[in] msg
 * @return none
 */
void OneNetDevice::MsgRespondActiveDeviceConnect(const SprMsg& msg)
{
    int16_t devNameLen = mOneDevName.length();
    int16_t productIdLen = mOneProductID.length();
    int16_t tokenLen = mOneToken.length();

    std::string payload;
    payload.push_back((devNameLen >> 8) & 0xFF);
    payload.push_back(devNameLen & 0xFF);
    payload.append(mOneDevName);
    payload.push_back((productIdLen >> 8) & 0xFF);
    payload.push_back(productIdLen & 0xFF);
    payload.append(mOneProductID);
    payload.push_back((tokenLen >> 8) & 0xFF);
    payload.push_back(tokenLen & 0xFF);
    payload.append(mOneToken);

    SprMsg conMsg(SIG_ID_ONENET_DRV_MQTT_MSG_CONNECT);
    conMsg.SetU32Value(mKeepAliveIntervalInSec);
    conMsg.SetString(payload);
    NotifyObserver(MODULE_ONENET_DRIVER, conMsg);
}

/**
 * @brief Process SIG_ID_ONENET_MGR_SET_CONNECT_STATUS
 *
 * @param[in] msg
 * @return none
 */
void OneNetDevice::MsgRespondSetConnectStatus(const SprMsg& msg)
{
    bool status = msg.GetBoolValue();
    mConnectStatus = status;
    SPR_LOGD("Connect status: %s\n", status ? "true" : "false");

    if (status) {
        SPR_LOGD("Device is ready, start send subscribe topics\n");
        StartSubscribeTopic();
    }
}

/**
 * @brief Process SIG_ID_ONENET_DEV_SUBSCRIBE_TOPIC
 *
 * @param[in] msg
 * @return none
 * tips：主题订阅支持通配符“+”（单层）和“#”（多层），实现同时订阅多个主题消息，例如：
 *  1、订阅全部物模型相关主题：$sys/{pid}/{device-name}/thing/#
 *  2、订阅物模型属性类相关主题：$sys/{pid}/{device-name}/thing/property/#
 *  3、订阅物模型服务调用类相关主题：
 *     $sys/{pid}/{device-name}/thing/service/#
 *     或者同时订阅$sys/{pid}/{device-name}/thing/service/+/invoke、$sys/{pid}/{device-name}/thing/service/+/invoke_reply
 *  4、订阅数据流模式下的命令下发：
 *     $sys/{pid}/{device-name}/cmd/#
 *     或者同时订阅$sys/{pid}/{device-name}/cmd/request/+、$sys/{pid}/{device-name}/cmd/response/+/accepted、$sys/{pid}/{device-name}/cmd/response/+/rejected
 *     订阅全部物模型相关主题：$sys/{pid}/{device-name}/thing/#
 */
void OneNetDevice::MsgRespondSubscribeTopic(const SprMsg& msg)
{
    if (mCurSubscribeIdx >= (int32_t)mAllTopics.size()) {
        SPR_LOGD("All topics are subscribed\n");
        return;
    }

    SprMsg tmpMsg(SIG_ID_ONENET_DRV_MQTT_MSG_SUBSCRIBE);
    tmpMsg.SetU16Value(GetUnusedIdentity());
    tmpMsg.SetString(mAllTopics[mCurSubscribeIdx]);
    NotifyObserver(MODULE_ONENET_DRIVER, tmpMsg);
    mCurSubscribeIdx++;
}

/**
 * @brief Process SIG_ID_ONENET_MGR_PING_TIMER_EVENT
 *
 * @param[in] msg
 * @return none
 */
void OneNetDevice::MsgRespondPingTimerEvent(const SprMsg& msg)
{
    SprMsg pingMsg(SIG_ID_ONENET_DRV_MQTT_MSG_PINGREQ);
    NotifyObserver(MODULE_ONENET_DRIVER, pingMsg);
}

int32_t OneNetDevice::ProcessMsg(const SprMsg& msg)
{
    SPR_LOGD("Recv msg: %s\n", GetSigName(msg.GetMsgId()));
    switch (msg.GetMsgId())
    {
        case SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT:
        {
            MsgRespondActiveDeviceConnect(msg);
            break;
        }
        case SIG_ID_ONENET_MGR_SET_CONNECT_STATUS:
        {
            MsgRespondSetConnectStatus(msg);
            break;
        }
        case SIG_ID_ONENET_DEV_SUBSCRIBE_TOPIC:
        {
            MsgRespondSubscribeTopic(msg);
            break;
        }
        case SIG_ID_ONENET_MGR_PING_TIMER_EVENT:
        {
            MsgRespondPingTimerEvent(msg);
            break;
        }
        default:
        {
            break;
        }
    }
    return 0;
}
