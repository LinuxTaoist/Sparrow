/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M08_Subscribe.cpp
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
#include "M08_Subscribe.h"

MqttSubscribe::MqttSubscribe(uint16_t identifier, const std::string& topic)
    : MqttMsgBase(MQTT_MSG_SUBSCRIBE, 2), mIdentifier(identifier), mTopic(topic)
{
}

MqttSubscribe::~MqttSubscribe()
{
}

int32_t MqttSubscribe::DecodeVariableHeader(const std::string& bytes)
{
    int len = 0;
    CHECK_ADD_RESULT(DecodeIntegerFromBytes(mIdentifier, bytes), len);
    return len;
}

int32_t MqttSubscribe::DecodePayload(const std::string& bytes)
{
    int8_t qos = 0;
    int32_t len = 0;
    uint16_t topicLength = 0;

    CHECK_ADD_RESULT(DecodeIntegerFromBytes(topicLength, bytes), len);
    CHECK_ADD_RESULT(DecodeU8BytesFromBytes(mTopic, bytes.substr(len), topicLength), len);
    CHECK_ADD_RESULT(DecodeIntegerFromBytes(qos, bytes.substr(len)), len);

    return len;
}

int32_t MqttSubscribe::EncodeVariableHeader(std::string& bytes)
{
    int32_t len = 0;

    mVariableHeader.clear();
    CHECK_ADD_RESULT(EncodeIntegerToBytes(mIdentifier, mVariableHeader), len);
    bytes += mVariableHeader;

    return len;
}

int32_t MqttSubscribe::EncodePayload(std::string& bytes)
{
    uint8_t qos = 0;
    int32_t len = 0;

    mPayload.clear();
    uint16_t topicLength = (uint16_t)mTopic.length();
    CHECK_ADD_RESULT(EncodeIntegerToBytes(topicLength, mPayload), len);
    CHECK_ADD_RESULT(EncodeU8BytesToBytes(mTopic, mPayload), len);
    CHECK_ADD_RESULT(EncodeIntegerToBytes(qos, mPayload), len);
    bytes += mPayload;

    return len;
}
