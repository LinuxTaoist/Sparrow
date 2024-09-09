/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : MqttMsg.h
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
#ifndef __MQTT_MSG_H__
#define __MQTT_MSG_H__

#include <string>
#include <vector>
#include <stdint.h>

#define CHECK_RESULT(expr) do { \
    if ((expr) == -1) {         \
        return -1;            \
    }                           \
} while(0)

#define CHECK_ADD_RESULT(expr, ret) do { \
    if ((expr) == -1) {         \
        return -1;              \
    } else {                    \
        ret += expr;            \
    }                           \
} while(0)

enum EMqttMsgType
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

#define MQTT_MSG_FLAG_QOS_MASK  0x06
#define MQTT_MSG_FLAG_QOS_SHIFT 1

enum EMqttSubackResult
{
    MQTT_SUBACK_SUCCESS = 0x00,
    MQTT_SUBACK_QOS0    = 0x01,
    MQTT_SUBACK_QOS1    = 0x02,
    MQTT_SUBACK_FAILURE = 0x80,
};

class MqttMsgBase
{
public:
    /**
     * @brief Construct/Destruct
     *
     * @param[in] type The type of the MQTT message
     * @param[in] flags The flags of the MQTT message
     */
    MqttMsgBase();
    MqttMsgBase(const std::string& bytes);
    MqttMsgBase(uint8_t type, uint8_t flags = 0);
    MqttMsgBase(uint8_t type, uint8_t flags, const std::string& payload);
    MqttMsgBase(const MqttMsgBase& msg);
    MqttMsgBase& operator=(const MqttMsgBase& msg);
    MqttMsgBase(MqttMsgBase&& msg);
    MqttMsgBase& operator=(MqttMsgBase&& msg);
    virtual ~MqttMsgBase();

    /**
     * @brief Set/Get the body bytes of the MQTT message
     *
     * @param[in] type The type of the MQTT message
     * @param[in] flags The flags of the MQTT message
     * @param[in] variableHeader The variable header of the MQTT message
     * @param[in] payload The payload of the MQTT message
     * @return length on success, or -1 if an error occurred
     */
    virtual int32_t SetFixedHeader(uint8_t type, uint8_t flags);
    virtual int32_t GetFixedHeader(uint8_t& type, uint8_t& flags);
    virtual int32_t SetVariableHeader(const std::string& variableHeader);
    virtual int32_t GetVariableHeader(std::string& variableHeader);
    virtual int32_t SetPayload(const std::string& payload);
    virtual int32_t GetPayload(std::string& payload);

    /**
     * @brief Decode/Encode the MQTT message from/to bytes
     *
     * @param[in] bytes The string to decode
     * @return length on success, or -1 if an error occurred
     */
    virtual int32_t Decode(const std::string& bytes);
    virtual int32_t Encode(std::string& bytes);

protected:
    // --------------------------------------------------------------------------------------------
    // Encode/Decode MQTT protocol functions
    // --------------------------------------------------------------------------------------------
    virtual int32_t DecodeRemainingLength(const std::string& bytes);
    virtual int32_t DecodeFixedHeader(const std::string& bytes);
    virtual int32_t DecodeVariableHeader(const std::string& bytes);
    virtual int32_t DecodePayload(const std::string& bytes);

    virtual int32_t EncodeRemainingLength(std::string& bytes);
    virtual int32_t EncodeFixedHeader(std::string& bytes);
    virtual int32_t EncodeVariableHeader(std::string& bytes);
    virtual int32_t EncodePayload(std::string& bytes);

    // --------------------------------------------------------------------------------------------
    // Encode/Decode common type functions
    // --------------------------------------------------------------------------------------------
    int32_t DecodeU8BytesFromBytes(std::string& data, const std::string& bytes, int32_t len = -1);
    template <typename T>
    int32_t DecodeIntegerFromBytes(T& data, const std::string& bytes)
    {
        size_t len = sizeof(T);
        if (len > bytes.size())
        {
            return -1;
        }

        data = 0;
        for (size_t i = 0; i < len; i++) {
            data <<= 8;
            data |= static_cast<uint8_t>(bytes[i]);
        }

        return len;
    }

    int32_t EncodeU8BytesToBytes(const std::string& data, std::string& bytes, int32_t len = -1);
    template <typename T>
    int32_t EncodeIntegerToBytes(const T& data, std::string& bytes)
    {
        size_t len = sizeof(T);
        for (size_t i = 0; i < len; i++) {
            char ch = static_cast<char>((data >> ((len - 1 - i) * 8)) & 0xFF);
            bytes.push_back(ch);
        }

        return len;
    }

protected:
    struct FixHeader
    {
        uint8_t     type  : 4;
        uint8_t     flags : 4;
        std::string remainingLength;
        FixHeader(uint8_t type, uint8_t flags) : type(type), flags(flags) {};
    } mFixedHeader;                 // Fixed Header in MQTT protocol
    std::string mVariableHeader;    // Variable Header in MQTT protocol
    std::string mPayload;           // Payload in MQTT protocol
    uint64_t mRemainLenValue;
};

#endif // __MQTT_MSG_H__
