/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M01_Connect.cpp
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
#include "M01_Connect.h"

MqttConnect::MqttConnect(std::string& protocolName, uint8_t version, uint8_t flags, uint16_t keepalive)
    : MqttMsgBase(MQTT_MSG_CONNECT, 0)
{
    mProtocolNameLength = protocolName.length();
    mProtocolName = protocolName;
    mProtocolVersion = version;
    mConnectFlags = flags;
    mKeepAlive = keepalive;
}

MqttConnect::~MqttConnect()
{
}

int32_t MqttConnect::Encode(std::string& bytes)
{
    // 1. mFixedHeader filled in Construct
    // 2. mVariableHeader filled in Encode of derived class, such as MqttConnect
    // 3. mPayload filled in SetPayload by caller
    mVariableHeader.clear();
    CHECK_RESULT(EncodeIntegerToBytes(mProtocolNameLength, mVariableHeader));
    CHECK_RESULT(EncodeU8BytesToBytes(mProtocolName, mVariableHeader));
    CHECK_RESULT(EncodeIntegerToBytes(mProtocolVersion, mVariableHeader));
    CHECK_RESULT(EncodeIntegerToBytes(mConnectFlags, mVariableHeader));
    CHECK_RESULT(EncodeIntegerToBytes(mKeepAlive, mVariableHeader));

    return MqttMsgBase::Encode(bytes);
}

void MqttConnect::SetProtocolName(const std::string &name)
{
    mProtocolNameLength = name.length();
    mProtocolName = name;
}

void MqttConnect::SetProtocolVersion(uint8_t version)
{
    mProtocolVersion = version;
}

void MqttConnect::SetConnectFlags(uint8_t flags)
{
    mConnectFlags = flags;
}

void MqttConnect::SetKeepAlive(uint16_t keepalive)
{
    mKeepAlive = keepalive;
}
