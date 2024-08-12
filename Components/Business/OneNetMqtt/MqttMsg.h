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

class MqttMsgBase
{
public:
    MqttMsgBase() {}
    virtual ~MqttMsgBase() {}

    virtual int32_t encode();
    virtual int32_t decode();
    virtual int32_t SetTopic(const std::string& topic);
    virtual int32_t GetTopic(std::string& topic) const;
    virtual int32_t SetPayload(const std::string& payload);
    virtual int32_t GetPayload(std::string& payload) const;

protected:
    int32_t decodeU32(uint32_t  data);
    int32_t encodeU32(uint32_t& data);
    int32_t decodeU16(uint16_t  data);
    int32_t encodeU16(uint16_t& data);
    int32_t decodeU8(uint8_t  data);
    int32_t encodeU8(uint8_t& data);
    int32_t decodeVectorU8(std::vector<uint8_t>& data, int32_t len);
    int32_t decodeVectorU16(std::vector<uint16_t>& data, int32_t len);

private:
    std::string mTopic;
    std::string mPayload;
};

#endif // __MQTT_MSG_H__
