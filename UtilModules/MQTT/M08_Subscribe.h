/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M08_Subscribe.h
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
#ifndef __M08_SUBSCRIBE_H__
#define __M08_SUBSCRIBE_H__

#include "MqttMsg.h"

class MqttSubscribe : public MqttMsgBase
{
public:
    MqttSubscribe(uint16_t identifier, const std::string& topic);
    ~MqttSubscribe();

    uint16_t GetPacketIdentifier() { return mPacketIdentifier; }

protected:
    int32_t EncodeVariableHeader(std::string& bytes) override;
    int32_t EncodePayload(std::string& bytes) override;

private:
    uint16_t mPacketIdentifier;
};

#endif // __M08_SUBSCRIBE_H__
