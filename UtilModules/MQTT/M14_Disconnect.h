/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M14_Disconnect.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/09/10
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __M14_DISCONNECT_H__
#define __M14_DISCONNECT_H__

#include <string>
#include "MqttMsg.h"

// 3.1 DISCONNECT – 连接服务端
class MqttDisconnect : public MqttMsgBase {
public:
    MqttDisconnect();
    virtual ~MqttDisconnect();
};

#endif // __M14_DISCONNECT_H__
