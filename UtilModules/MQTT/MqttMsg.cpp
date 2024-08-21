/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : MqttMsg.cpp
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
#include "SprLog.h"
#include "MqttMsg.h"

#define SPR_LOGI(fmt, args...) LOGI("MqttMsgBase", fmt, ##args)
#define SPR_LOGD(fmt, args...) LOGD("MqttMsgBase", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("MqttMsgBase", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("MqttMsgBase", fmt, ##args)

MqttMsgBase::MqttMsgBase() : mFixedHeader(0, 0)
{
}

MqttMsgBase::MqttMsgBase(uint8_t type, uint8_t flags)
    : mFixedHeader(type, flags)
{
}

MqttMsgBase::MqttMsgBase(const std::string& bytes)
    : mFixedHeader(0, 0)
{
    Decode(bytes);
}

MqttMsgBase::MqttMsgBase(const MqttMsgBase& msg)
    : mFixedHeader(msg.mFixedHeader.type, msg.mFixedHeader.flags),
      mVariableHeader(msg.mVariableHeader),
      mPayload(msg.mPayload)
{
}

MqttMsgBase& MqttMsgBase::operator=(const MqttMsgBase& msg)
{
    if (this != &msg)
    {
        mFixedHeader = msg.mFixedHeader;
        mVariableHeader = msg.mVariableHeader;
        mPayload = msg.mPayload;
    }
    return *this;
}

MqttMsgBase::MqttMsgBase(MqttMsgBase&& msg)
    : mFixedHeader(std::move(msg.mFixedHeader)),
      mVariableHeader(std::move(msg.mVariableHeader)),
      mPayload(std::move(msg.mPayload))
{
}

MqttMsgBase& MqttMsgBase::operator=(MqttMsgBase&& msg)
{
    if (this != &msg)
    {
        mFixedHeader = std::move(msg.mFixedHeader);
        mVariableHeader = std::move(msg.mVariableHeader);
        mPayload = std::move(msg.mPayload);
    }
    return *this;
}

MqttMsgBase::~MqttMsgBase()
{
}

int32_t MqttMsgBase::SetFixedHeader(uint8_t type, uint8_t flags)
{
    mFixedHeader.type = type;
    mFixedHeader.flags = flags;
    return 0;
}

int32_t MqttMsgBase::GetFixedHeader(uint8_t& type, uint8_t& flags)
{
    type = mFixedHeader.type;
    flags = mFixedHeader.flags;
    return 0;
}

int32_t MqttMsgBase::SetVariableHeader(const std::string& variableHeader)
{
    mVariableHeader = variableHeader;
    return 0;
}

int32_t MqttMsgBase::GetVariableHeader(std::string& variableHeader)
{
    variableHeader = mVariableHeader;
    return 0;
}

int32_t MqttMsgBase::SetPayload(const std::string& payload)
{
    mPayload = payload;
    return 0;
}

int32_t MqttMsgBase::GetPayload(std::string& payload)
{
    payload = mPayload;
    return 0;
}

int32_t MqttMsgBase::Decode(const std::string& bytes)
{
    int32_t fixLen = DecodeFixedHeader(bytes);
    if (fixLen < 0) {
        return fixLen;
    }

    int varLen = DecodeVariableHeader(bytes.substr(fixLen));
    if (varLen < 0) {
        return varLen;
    }

    int32_t payLen = DecodePayload(bytes.substr(fixLen + varLen));
    if (payLen < 0) {
        return payLen;
    }

    SPR_LOGD("fixLen = %d, varLen = %d, payLen = %d\n", fixLen, varLen, payLen);
    return fixLen + varLen + payLen;
}

int32_t MqttMsgBase::Encode(std::string& bytes)
{
    int32_t fixLen = EncodeFixedHeader(bytes);
    if (fixLen < 0) {
        return fixLen;
    }

    int varLen = EncodeVariableHeader(bytes);
    if (varLen < 0) {
        return varLen;
    }

    int32_t payLen = EncodePayload(bytes);
    if (payLen < 0) {
        return payLen;
    }

    return fixLen + varLen + payLen;
}

int32_t MqttMsgBase::DecodeRemainingLength(const std::string& bytes)
{
    int32_t len = 0;
    uint64_t value = 0;
    uint64_t multiplier = 1;
    for (auto byte : bytes) {
        len++;
        value += (byte & 127) * multiplier;

        multiplier *= 128;
        if (multiplier > 128 * 128 * 128) {
            return -1; // Too long.
        }
        if ((byte & 128) == 0) {    // Last byte.
            break;
        }
    }

    return len;
}

int32_t MqttMsgBase::EncodeRemainingLength(std::string& bytes)
{
    int32_t len = 0;
    uint64_t x = mVariableHeader.length() + mPayload.length();
    do {
        uint8_t encodedByte = (uint8_t)(x % 128);
        x /= 128;
        if (x > 0) {
            encodedByte |= 0x80;
        }

        bytes.push_back(encodedByte & 0xFF);
        len++;
    } while (x > 0);

    return len;
}

int32_t MqttMsgBase::DecodeFixedHeader(const std::string& bytes)
{
    uint8_t byte = 0;
    int32_t tfLen = DecodeIntegerFromBytes(byte, bytes);
    if (tfLen < 0) {
        return tfLen;
    }

    int32_t rLen = DecodeRemainingLength(bytes.substr(tfLen));
    if (rLen < 0) {
        return rLen;
    }

    mFixedHeader.type = byte >> 4;
    mFixedHeader.flags = byte & 0x0F;
    return tfLen + rLen;
}

int32_t MqttMsgBase::EncodeFixedHeader(std::string& bytes)
{
    uint8_t byte = mFixedHeader.type << 4 | mFixedHeader.flags;
    int tfLen = EncodeIntegerToBytes(byte, bytes);
    if (tfLen < 0) {
        return tfLen;
    }

    int rLen = EncodeRemainingLength(bytes);
    if (rLen < 0) {
        return rLen;
    }

    return 1 + rLen;
}

int32_t MqttMsgBase::DecodeVariableHeader(const std::string& data)
{
    uint16_t conLen = 0;    // Decode MSB(8) & LSB(8)
    int32_t lLen = DecodeIntegerFromBytes(conLen, data);
    if (lLen < 0) {
        return lLen;
    }

    mVariableHeader = data.substr(conLen + 2, conLen);
    return 2 + conLen;
}

int32_t MqttMsgBase::EncodeVariableHeader(std::string& bytes)
{
    bytes.append(mVariableHeader);
    return bytes.length();
}

int32_t MqttMsgBase::DecodePayload(const std::string& data)
{
    mPayload = data;
    return mPayload.length();
}

int32_t MqttMsgBase::EncodePayload(std::string& data)
{
    data.append(mPayload);
    return mPayload.length();
}

int32_t MqttMsgBase::DecodeU8BytesFromBytes(std::string& value, const std::string& bytes, int32_t len)
{
    if (len == -1) {
        len = bytes.length();
    }

    value.assign(bytes.begin(), bytes.begin() + len);
    return len;
}

int32_t MqttMsgBase::EncodeU8BytesToBytes(const std::string& data, std::string& bytes, int32_t len)
{
    if (len == -1) {
        len = data.length();
    }

    bytes.append(data.begin(), data.begin() + len);
    return len;
}
