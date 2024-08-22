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
    : MqttMsgBase(MQTT_MSG_SUBSCRIBE, 2), mPacketIdentifier(identifier)
{
    mVariableHeader.push_back((identifier >> 8) & 0xFF);
    mVariableHeader.push_back(identifier & 0xFF);
    uint16_t topicLength = (uint16_t)topic.length();
    mPayload.push_back((topicLength >> 8) & 0xFF);
    mPayload.push_back(topicLength & 0xFF);
    mPayload += topic;
    mPayload.push_back(0x00); // QoS level
}

MqttSubscribe::~MqttSubscribe()
{
}
