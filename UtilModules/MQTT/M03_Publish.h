/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M03_Publish.h
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
#ifndef __M03_PUBLISH_H__
#define __M03_PUBLISH_H__

#include "MqttMsg.h"

class MqttPublish : public MqttMsgBase
{
public:
    MqttPublish();
    MqttPublish(uint8_t flags, uint16_t identifier, const std::string& topic, const std::string& payload);
    virtual ~MqttPublish();

    std::string GetTopic() const { return mTopic; }

protected:
    int32_t DecodeVariableHeader(const std::string& bytes) override;
    int32_t EncodeVariableHeader(std::string& bytes) override;

private:
    uint16_t mIdentifier;
    std::string mTopic;
};

#endif // __M03_PUBLISH_H__