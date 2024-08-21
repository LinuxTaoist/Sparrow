/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M01_Connack.cpp
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
#include "M02_Connack.h"

MqttConnack::MqttConnack()
    : MqttMsgBase(MQTT_MSG_CONNACK, 0), mConnAckStatus(0), mReturnCode(0)
{
}

MqttConnack::MqttConnack(const std::string& bytes)
    : MqttMsgBase(MQTT_MSG_CONNACK, 0), mConnAckStatus(0), mReturnCode(0)
{
}

MqttConnack::~MqttConnack()
{
}

int32_t MqttConnack::DecodeVariableHeader(const std::string& bytes)
{
    int len1 = DecodeIntegerFromBytes(mConnAckStatus, bytes);
    CHECK_RESULT(len1);
    int len2 = DecodeIntegerFromBytes(mReturnCode, bytes.substr(len1));
    CHECK_RESULT(len2);

    return len1 + len2;
}

int32_t MqttConnack::GetConnAckStatus() const
{
    return mConnAckStatus;
}

int32_t MqttConnack::GetReturnCode() const
{
    return mReturnCode;
}
