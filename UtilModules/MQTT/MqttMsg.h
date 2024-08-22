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
    int32_t _ret = (expr);      \
    if (_ret == -1) {           \
        return _ret;            \
    }                           \
} while(0)

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
     * @brief Set the Fixed Header object
     *
     * @param[in] type The type of the MQTT message
     * @param[in] flags The flags of the MQTT message
     * @return length on success, or -1 if an error occurred
     */
    virtual int32_t SetFixedHeader(uint8_t type, uint8_t flags);

    /**
     * @brief Get the Fixed Header object
     *
     * @param[out] type The type of the MQTT message
     * @param[out] flags The flags of the MQTT message
     * @return length on success, or -1 if an error occurred
     */
    virtual int32_t GetFixedHeader(uint8_t& type, uint8_t& flags);

    /**
     * @brief Set the Variable Header object
     *
     * @param[in] variableHeader The Variable Header object
     * @return length on success, or -1 if an error occurred
     */
    virtual int32_t SetVariableHeader(const std::string& variableHeader);

    /**
     * @brief Get the Variable Header object
     *
     * @param[out] variableHeader The Variable Header object
     * @return length on success, or -1 if an error occurred
     */
    virtual int32_t GetVariableHeader(std::string& variableHeader);

    /**
     * @brief Set the Payload object
     *
     * @param[in] payload The Payload object
     * @return length on success, or -1 if an error occurred
     */
    virtual int32_t SetPayload(const std::string& payload);

    /**
     * @brief Get the Payload object
     *
     * @param payload The Payload object
     * @return length on success, or -1 if an error occurred
     */
    virtual int32_t GetPayload(std::string& payload);

    /**
     * @brief Decode the MQTT message from a string
     *
     * @param[in] bytes The string to decode
     * @return length on success, or -1 if an error occurred
     */
    virtual int32_t Decode(const std::string& bytes);

    /**
     * @brief Encode the MQTT message to a string
     *
     * @param[in] bytes The string to store the encoded message
     * @return length on success, or -1 if an error occurred
     */
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
