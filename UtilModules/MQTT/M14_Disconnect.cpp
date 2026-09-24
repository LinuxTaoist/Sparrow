/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M14_Disconnect.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/09/10
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "M14_Disconnect.h"

MqttDisconnect::MqttDisconnect() : MqttMsgBase(MQTT_MSG_DISCONNECT, 0) {
}

MqttDisconnect::~MqttDisconnect() {
}
