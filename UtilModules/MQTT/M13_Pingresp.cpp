/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M13_Pingresp.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/20
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "M13_Pingresp.h"

MqttPingresq::MqttPingresq() : MqttMsgBase(MQTT_MSG_PINGRESP, 2) {
}

MqttPingresq::~MqttPingresq() {
}
