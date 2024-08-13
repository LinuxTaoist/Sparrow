/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : MqttDriver.h
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
#ifndef __MQTT_DRIVER_H__
#define __MQTT_DRIVER_H__

#include <string>
#include "SprObserver.h"

class MqttMsgBase
{
public:
    MqttMsgBase();
    ~MqttMsgBase();
};

class MqttDriver : public SprObserver
{
public:
    MqttDriver();
    ~MqttDriver();
    void Init();

private:
    std::string mMqttAddress;
    uint16_t    mMqttPort;
};

#endif // __MQTT_DRIVER_H__
