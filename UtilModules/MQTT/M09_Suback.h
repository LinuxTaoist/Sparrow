/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M09_Suback.h
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
#ifndef __M09_SUBACK_H__
#define __M09_SUBACK_H__

#include "MqttMsg.h"

class Suback : public MqttMsgBase
{
public:
    Suback();
    ~Suback();

    int32_t DecodeVariableHeader(const std::string& bytes) override;
    int32_t DecodePayload(const std::string& bytes) override;

    uint16_t GetPacketIdentifier() { return mIdentifier; }
    uint8_t GetReturnCode() { return mReturnCode; }

private:
    uint16_t mIdentifier;   // 2 BYTE   报文标识符
    uint8_t mReturnCode;    // 1 BYTE   返回码
};

#endif // __M09_SUBACK_H__
