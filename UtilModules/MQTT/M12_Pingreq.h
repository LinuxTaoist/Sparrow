/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M12_Pingreq.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/20
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __M12_PINGREQ_H__
#define __M12_PINGREQ_H__

#include "MqttMsg.h"

// 3.12 PINGREQ – 心跳请求
class MqttPingreq : public MqttMsgBase {
public:
    MqttPingreq();
    virtual ~MqttPingreq();
};

#endif // __M12_PINGREQ_H__
