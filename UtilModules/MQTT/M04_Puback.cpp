/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M03_Puback.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/09/11
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/09/11 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "M04_Puback.h"

MqttPuback::MqttPuback()
    : MqttMsgBase(MQTT_MSG_PUBACK, 0)
{
    mIdentifier = 0;
}

MqttPuback::MqttPuback(uint8_t flags, uint16_t identifier)
    : MqttMsgBase(MQTT_MSG_PUBACK, flags), mIdentifier(identifier)
{
}

MqttPuback::~MqttPuback()
{
}

int32_t MqttPuback::DecodeVariableHeader(const std::string& bytes)
{
    return DecodeIntegerFromBytes(mIdentifier, bytes);
}

int32_t MqttPuback::EncodeVariableHeader(std::string& bytes)
{
    int32_t len = 0;

    mVariableHeader.clear();
    CHECK_ADD_RESULT(EncodeIntegerToBytes(mIdentifier, mVariableHeader), len);
    bytes += mVariableHeader;

    return len;
}
