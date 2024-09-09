/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M01_Connack.h
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
#ifndef __M02_CONNACK_H__
#define __M02_CONNACK_H__

#include "MqttMsg.h"

class MqttConnack : public MqttMsgBase
{
public:
    MqttConnack();
    MqttConnack(const std::string& bytes);
    virtual ~MqttConnack();

    int32_t GetConnAckStatus() const;
    int32_t GetReturnCode() const;

protected:
    int32_t DecodeVariableHeader(const std::string& bytes) override;

private:
    uint8_t mConnAckStatus;         // 1 BYTE   连接确认标志
    uint8_t mReturnCode;            // 1 BYTE   连接返回码

};

#endif // __M02_CONNACK_H__
