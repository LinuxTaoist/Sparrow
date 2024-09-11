/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M03_Puback.h
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
#ifndef __M04_PUBACK_H__
#define __M04_PUBACK_H__

#include "MqttMsg.h"

class MqttPuback : public MqttMsgBase
{
public:
    MqttPuback();
    MqttPuback(uint8_t flags, uint16_t identifier);
    virtual ~MqttPuback();

protected:
    int32_t DecodeVariableHeader(const std::string& bytes) override;
    int32_t EncodeVariableHeader(std::string& bytes) override;

private:
    uint16_t mIdentifier;
};

#endif // __M04_PUBACK_H__