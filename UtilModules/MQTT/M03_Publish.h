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
    ~MqttPublish();

    void SetTopic(const char *topic);
    void SetPayload(const char *payload);
    void SetQos(uint8_t qos);
    void SetRetain(bool retain);

    void SetTopic(const char *topic, uint16_t topic_len);
    void SetPayload(const char *payload, uint16_t payload_len);

private:
    std::string mTopic;
};

#endif // __M03_PUBLISH_H__