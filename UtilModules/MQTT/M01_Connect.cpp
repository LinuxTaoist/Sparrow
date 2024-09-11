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

int32_t MqttConnect::DecodeVariableHeader(const std::string& bytes)
{
    int32_t len = 0;

    CHECK_ADD_RESULT(DecodeIntegerFromBytes(mProtocolNameLength, bytes), len);
    CHECK_ADD_RESULT(DecodeU8BytesFromBytes(mProtocolName, bytes.substr(len), mProtocolNameLength), len);
    CHECK_ADD_RESULT(DecodeIntegerFromBytes(mProtocolVersion, bytes.substr(len)), len);
    CHECK_ADD_RESULT(DecodeIntegerFromBytes(mConnectFlags, bytes.substr(len)), len);
    CHECK_ADD_RESULT(DecodeIntegerFromBytes(mKeepAlive, bytes.substr(len)), len);

    return len;
}

int32_t MqttConnect::EncodeVariableHeader(std::string& bytes)
{
    int32_t len = 0;

    mVariableHeader.clear();
    CHECK_ADD_RESULT(EncodeIntegerToBytes(mProtocolNameLength, mVariableHeader), len);
    CHECK_ADD_RESULT(EncodeU8BytesToBytes(mProtocolName, mVariableHeader), len);
    CHECK_ADD_RESULT(EncodeIntegerToBytes(mProtocolVersion, mVariableHeader), len);
    CHECK_ADD_RESULT(EncodeIntegerToBytes(mConnectFlags, mVariableHeader), len);
    CHECK_ADD_RESULT(EncodeIntegerToBytes(mKeepAlive, mVariableHeader), len);

    bytes += mVariableHeader;
    return len;
}
