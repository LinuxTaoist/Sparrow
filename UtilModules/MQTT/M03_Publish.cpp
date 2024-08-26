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

MqttPublish::MqttPublish(uint8_t flags, uint16_t identifier, const std::string& topic, const std::string& payload)
    : MqttMsgBase(MQTT_MSG_PUBLISH, flags)
{
    // Encode variable header
    uint16_t topicLen = topic.length();
    EncodeIntegerToBytes(topicLen,      mVariableHeader);
    EncodeU8BytesToBytes(topic,         mVariableHeader);

    uint8_t qosLevel = (flags & MQTT_MSG_FLAG_QOS_MASK) >> MQTT_MSG_FLAG_QOS_SHIFT;
    if (qosLevel == MQTT_SUBACK_QOS0 || qosLevel == MQTT_SUBACK_QOS1) {
        EncodeIntegerToBytes(identifier,    mVariableHeader);
    }

    // Encode payload
    EncodeU8BytesToBytes(payload, mPayload);
}

MqttPublish::~MqttPublish()
{
}
