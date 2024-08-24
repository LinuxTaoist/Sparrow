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

    mVariableHeader.clear();
    EncodeIntegerToBytes(mProtocolNameLength, mVariableHeader);
    EncodeU8BytesToBytes(mProtocolName, mVariableHeader);
    EncodeIntegerToBytes(mProtocolVersion, mVariableHeader);
    EncodeIntegerToBytes(mConnectFlags, mVariableHeader);
    EncodeIntegerToBytes(mKeepAlive, mVariableHeader);
}

MqttConnect::~MqttConnect()
{
}
