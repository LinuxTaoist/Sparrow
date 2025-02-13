/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M03_Publish.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/20
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/08/20 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "M03_Publish.h"

MqttPublish::MqttPublish()
    : MqttMsgBase(MQTT_MSG_PUBLISH)
{
}

MqttPublish::MqttPublish(uint8_t flags, uint16_t identifier, const std::string& topic, const std::string& payload)
    : MqttMsgBase(MQTT_MSG_PUBLISH, flags, payload), mIdentifier(identifier), mTopic(topic)
{
}

MqttPublish::~MqttPublish()
{
}

int32_t MqttPublish::DecodeVariableHeader(const std::string& bytes)
{
    int32_t len = 0;
    uint16_t topicLen = 0;
    CHECK_ADD_RESULT(DecodeIntegerFromBytes(topicLen, bytes), len);
    CHECK_ADD_RESULT(DecodeU8BytesFromBytes(mTopic, bytes.substr(len), topicLen), len);
    return len;
}

int32_t MqttPublish::EncodeVariableHeader(std::string& bytes)
{
    int32_t len = 0;
    uint16_t topicLen = mTopic.length();

    mVariableHeader.clear();
    CHECK_ADD_RESULT(EncodeIntegerToBytes(topicLen, mVariableHeader), len);
    CHECK_ADD_RESULT(EncodeU8BytesToBytes(mTopic, mVariableHeader), len);
    uint8_t qosLevel = (mFixedHeader.flags & MQTT_MSG_FLAG_QOS_MASK) >> MQTT_MSG_FLAG_QOS_SHIFT;
    if (qosLevel == MQTT_SUBACK_QOS0 || qosLevel == MQTT_SUBACK_QOS1) {
        CHECK_ADD_RESULT(EncodeIntegerToBytes(mIdentifier, mVariableHeader), len);
    }

    bytes += mVariableHeader;
    return len;
}
