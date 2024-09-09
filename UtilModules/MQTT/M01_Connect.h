/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M01_Connect.h
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
#ifndef __M01_CONNECT_H__
#define __M01_CONNECT_H__

#include <string>
#include "MqttMsg.h"

// 3.1 CONNECT – 连接服务端
class MqttConnect : public MqttMsgBase
{
public:
    MqttConnect(std::string& protocolName, uint8_t version, uint8_t flags, uint16_t keepalive);
    virtual ~MqttConnect();

    uint16_t GetProtocolNameLength() { return mProtocolNameLength; };
    std::string GetProtocolName() { return mProtocolName; };
    uint8_t GetProtocolVersion() { return mProtocolVersion; };
    uint8_t GetConnectFlags() { return mConnectFlags; };
    uint16_t GetKeepAlive() { return mKeepAlive; };

private:
                                    // 可变包头
    uint16_t mProtocolNameLength;   // 2 BYTE  协议名长度
    std::string mProtocolName;      // N BYTE  协议名
    uint8_t mProtocolVersion;       // 1 BYTE  协议级别
    uint8_t mConnectFlags;          // 1 BYTE  连接标志
    uint16_t mKeepAlive;            // 2 BYTE  保持连接时间
};

#endif // __M01_CONNECT_H__
