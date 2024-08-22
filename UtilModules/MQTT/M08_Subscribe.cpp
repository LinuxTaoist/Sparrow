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
    : MqttMsgBase(MQTT_MSG_SUBSCRIBE, 2, topic), mPacketIdentifier(identifier)
{
}

MqttSubscribe::~MqttSubscribe()
{
}

int32_t MqttSubscribe::EncodeVariableHeader(std::string& bytes)
{
    return EncodeIntegerToBytes(mPacketIdentifier, bytes);
}

int32_t MqttSubscribe::EncodePayload(std::string& bytes)
{
    uint16_t topicLength = (uint16_t)mPayload.length();
    bytes.push_back((topicLength >> 8) & 0xFF);
    bytes.push_back(topicLength & 0xFF);
    bytes += mPayload;
    return 2 + topicLength;
}
