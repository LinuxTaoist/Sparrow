/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M13_Pingresq.h
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
#ifndef __M13_PINGRESP_H__
#define __M13_PINGRESP_H__

#include "MqttMsg.h"

// 3.13 PINGRESP – 心跳响应
class MqttPingresq : public MqttMsgBase
{
public:
    MqttPingresq();
    ~MqttPingresq();
};

#endif // __M13_PINGRESP_H__
