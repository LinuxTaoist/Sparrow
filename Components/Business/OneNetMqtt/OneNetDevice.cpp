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
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string.h>
#include "SprLog.h"
#include "cJSON.h"
#include "OneNetCommon.h"
#include "OneNetDevice.h"

using namespace InternalDefs;

#define SPR_LOGI(fmt, args...) LOGI("OneDev", "[%s] " fmt, mOneDevName.c_str(), ##args)
#define SPR_LOGD(fmt, args...) LOGD("OneDev", "[%s] " fmt, mOneDevName.c_str(), ##args)
#define SPR_LOGW(fmt, args...) LOGW("OneDev", "[%s] " fmt, mOneDevName.c_str(), ##args)
#define SPR_LOGE(fmt, args...) LOGE("OneDev", "[%s] " fmt, mOneDevName.c_str(), ##args)

#define ONENET_DEVICE_CFG_PATH          "OneNetDevices.conf"
#define DEFAULT_KEEP_ALIVE_INTERVAL     60  // 60s

#define TEMPLATE_TOPIC_THING            "$sys/%s/%s/thing/%s"
#define TEMPLATE_TOPIC_CMD              "$sys/%s/%s/cmd/%s"
#define TEMPLATE_TOPIC_THING_POST       "$sys/%s/%s/thing/property/post"

OneNetDevice::OneNetDevice(ModuleIDType id, const std::string& name)
    : SprObserverWithMQueue(id, name), mExpirationTime(0), mConnectStatus(false)
{
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
    char topicCmd[128] = {0};

    // $sys/{pid}/{device-name}/thing/property/#
    // $sys/{pid}/{device-name}/cmd/#
    snprintf(topicThingAll, sizeof(topicThingAll), TEMPLATE_TOPIC_THING, mOneProductID.c_str(), mOneDevName.c_str(), "property/post/reply");
    snprintf(topicCmd, sizeof(topicCmd), TEMPLATE_TOPIC_CMD, mOneProductID.c_str(), mOneDevName.c_str(), "#");
    mAllTopicMap[topicThingAll] = {TOPIC_STATUS_IDLE, 0, topicThingAll};
    mAllTopicMap[topicCmd] = {TOPIC_STATUS_IDLE, 0, topicCmd};

    for (const auto &topic : mAllTopicMap) {
        SPR_LOGI("OneNet Topic: [%s]\n", topic.first.c_str());
    }

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
    SprMsg msg(SIG_ID_ONENET_DEV_SUBSCRIBE_TOPIC);
    SendMsg(msg);
}

std::string OneNetDevice::PreparePublishPayloadJson()
{
    std::string payload;
    static int32_t id = 0;
    static int32_t value = 0;

    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "id", std::to_string(++id).c_str());
    cJSON_AddStringToObject(root, "version", "1.0");
    cJSON* params = cJSON_AddObjectToObject(root, "params");
    cJSON* power = cJSON_AddObjectToObject(params, "Power");
    cJSON_AddNumberToObject(power, "value", ++value);
    payload = cJSON_Print(root);
    cJSON_Delete(root);

    return payload;
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

    SPR_LOGD("Connect status: %s\n", status ? "true" : "false");
    mConnectStatus = status;
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
 *
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
    std::string tmpTopic;
    auto topicIt = std::find_if(mAllTopicMap.begin(), mAllTopicMap.end(),
        [](const std::pair<std::string, OneNetTopic>& topic) {
        return topic.second.status == TOPIC_STATUS_IDLE;
    });

    if (topicIt == mAllTopicMap.end()) {
        SPR_LOGD("All topics are subscribed\n");
        return;
    }

    uint16_t identity = GetUnusedIdentity();
    tmpTopic = topicIt->second.topic;
    topicIt->second.status = TOPIC_STATUS_WAIT_ACK;
    topicIt->second.identifier = identity;
    mCurTopic = tmpTopic;
    SPR_LOGD("subscribe topic: %s\n", tmpTopic.c_str());

    SprMsg tmpMsg(SIG_ID_ONENET_DRV_MQTT_MSG_SUBSCRIBE);
    tmpMsg.SetU16Value(identity);
    tmpMsg.SetString(tmpTopic);
    NotifyObserver(MODULE_ONENET_DRIVER, tmpMsg);
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_SUBACK
 *
 * @param[in] msg
 * @return none
 */
void OneNetDevice::MsgRespondSubscribeTopicAck(const SprMsg& msg)
{
    uint8_t retCode = msg.GetU8Value();
    uint16_t identity = msg.GetU16Value();
    auto topicIt = std::find_if(mAllTopicMap.begin(), mAllTopicMap.end(),
        [identity](const std::pair<std::string, OneNetTopic>& topic) {
        return topic.second.identifier == identity;
    });

    if (topicIt != mAllTopicMap.end()) {
        topicIt->second.status = (retCode != 0x80) ? TOPIC_STATUS_SUBSCRIBED_SUCCESS : TOPIC_STATUS_SUBSCRIBING_FAIL;
    }

    SPR_LOGD("Recv subscribe topic [%s] ack, id: %02X ret: %02X \n", mCurTopic.c_str(), identity, retCode);
    SprMsg tMsg(SIG_ID_ONENET_DEV_SUBSCRIBE_TOPIC);
    SendMsg(tMsg);
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

/**
 * @brief Process SIG_ID_ONENET_MGR_DATA_REPORT_TIMER_EVENT
 *
 * @param[in] msg
 * @return none
 */
void OneNetDevice::MsgRespondDataReportTimerEvent(const SprMsg& msg)
{
    char topicThingPost[100] = {0};
    std::string bodyJson = PreparePublishPayloadJson();
    uint16_t identity = GetUnusedIdentity();
    snprintf(topicThingPost, sizeof(topicThingPost), TEMPLATE_TOPIC_THING_POST, mOneProductID.c_str(), mOneDevName.c_str());

    auto pParam = std::make_shared<SOneNetPublishParam>();
    pParam->flags = 0x00;
    pParam->identifier = identity;
    strncpy(pParam->topic, topicThingPost, sizeof(pParam->topic) - 1);
    strncpy(pParam->payload, (char*)bodyJson.c_str(), sizeof(pParam->payload) - 1);

    SprMsg tMsg(SIG_ID_ONENET_DRV_MQTT_MSG_PUBLISH);
    tMsg.SetDatas(pParam, sizeof(SOneNetPublishParam));
    NotifyObserver(MODULE_ONENET_DRIVER, tMsg);
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
        case SIG_ID_ONENET_DRV_MQTT_MSG_SUBACK:
        {
            MsgRespondSubscribeTopicAck(msg);
            break;
        }
        case SIG_ID_ONENET_MGR_PING_TIMER_EVENT:
        {
            MsgRespondPingTimerEvent(msg);
            break;
        }
        case SIG_ID_ONENET_MGR_DATA_REPORT_TIMER_EVENT:
        {
            MsgRespondDataReportTimerEvent(msg);
            break;
        }
        default:
        {
            break;
        }
    }
    return 0;
}
