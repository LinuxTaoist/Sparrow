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
    : MqttMsgBase(MQTT_MSG_SUBSCRIBE, 2), mIdentifier(identifier)
{
    // Encode the variable header
    EncodeIntegerToBytes(identifier, mVariableHeader);

    // Encode the payload
    uint8_t qos = 0x00;
    uint16_t topicLength = (uint16_t)topic.length();
    EncodeIntegerToBytes(topicLength,   mPayload);
    EncodeU8BytesToBytes(topic,         mPayload);
    EncodeIntegerToBytes(qos,           mPayload);
}

MqttSubscribe::~MqttSubscribe()
{
}
