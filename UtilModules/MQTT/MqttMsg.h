/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : MqttMsg.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/12/21
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/12/21 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __MQTT_MSG_H__
#define __MQTT_MSG_H__

#include <string>
#include <vector>

enum MQTT_MSG_TYPE
{
    MQTT_MSG_CONNECT        = 1,
    MQTT_MSG_CONNACK        = 2,
    MQTT_MSG_PUBLISH        = 3,
    MQTT_MSG_PUBACK         = 4,
    MQTT_MSG_PUBREC         = 5,
    MQTT_MSG_PUBREL         = 6,
    MQTT_MSG_PUBCOMP        = 7,
    MQTT_MSG_SUBSCRIBE      = 8,
    MQTT_MSG_SUBACK         = 9,
    MQTT_MSG_UNSUBSCRIBE    = 10,
    MQTT_MSG_UNSUBACK       = 11,
    MQTT_MSG_PINGREQ        = 12,
    MQTT_MSG_PINGRESP       = 13,
    MQTT_MSG_DISCONNECT     = 14,
    MQTT_MSG_BUTT,
};

enum MQTT_RSP_CONNECT_CODE
{
    MQTT_CONNECT_ACCEPTED = 0,
    MQTT_CONNECT_REFUSED_UNACCEPTABLE_PROTOCOL_VERSION = 1,
    MQTT_CONNECT_REFUSED_IDENTIFIER_REJECTED = 2,
    MQTT_CONNECT_REFUSED_SERVER_UNAVAILABLE = 3,
    MQTT_CONNECT_REFUSED_BAD_USERNAME_OR_PASSWORD = 4,
    MQTT_CONNECT_REFUSED_NOT_AUTHORIZED = 5,
    MQTT_CONNECT_BUTT
};

class MqttMsgBase
{
public:
    MqttMsgBase();
    MqttMsgBase(const MqttMsgBase& msg);
    MqttMsgBase& operator=(const MqttMsgBase& msg);
    MqttMsgBase(MqttMsgBase&& msg);
    MqttMsgBase& operator=(MqttMsgBase&& msg);
    MqttMsgBase(uint8_t type, uint8_t flags = 0);
    virtual ~MqttMsgBase();

    virtual int32_t Encode(std::string& data);
    virtual int32_t Decode(const std::string& data);
    virtual int32_t SetFixedHeader(uint8_t type, uint8_t flags);
    virtual int32_t GetFixedHeader(uint8_t& type, uint8_t& flags);
    virtual int32_t SetVariableHeader(const std::string& variableHeader);
    virtual int32_t GetVariableHeader(std::string& variableHeader);
    virtual int32_t SetPayload(const std::string& payload);
    virtual int32_t GetPayload(std::string& payload);

protected:
    int32_t DecodeU32(uint32_t  data);
    int32_t EncodeU32(uint32_t& data);
    int32_t DecodeU16(uint16_t  data);
    int32_t EncodeU16(uint16_t& data);
    int32_t DecodeU8(uint8_t  data);
    int32_t EncodeU8(uint8_t& data);
    int32_t DecodeStringU8(std::string& data, int32_t len);
    int32_t EncodeStringU8(std::string& data);
    int32_t DecodeVectorU8(std::vector<uint8_t>& data, int32_t len);
    int32_t EncodeVectorU8(std::vector<uint8_t>& data);
    int32_t DecodeVectorU16(std::vector<uint16_t>& data, int32_t len);
    int32_t EncodeVectorU16(std::vector<uint16_t>& data);
    int32_t DecodeRemainingLength();
    int32_t EncodeRemainingLength();

private:
    struct Header
    {
        uint8_t     type  : 4;
        uint8_t     flags : 4;
        std::string remainingLength;
        Header(uint8_t type, uint8_t flags) : type(type), flags(flags) {};
    } mFixedHeader;

    std::string mVariableHeader;
    std::string mPayload;
    uint32_t    mPayloadIdx;
};

#endif // __MQTT_MSG_H__
