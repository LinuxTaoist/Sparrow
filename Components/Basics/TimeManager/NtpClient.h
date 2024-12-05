/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : NtpClient.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/11/21
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/11/21 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __NTP_CLIENT_H__
#define __NTP_CLIENT_H__

#include <memory>
#include <string>
#include <stdint.h>
#include "PSocket.h"

class NtpClient
{
public:
    NtpClient(const std::string addr, uint16_t port, const std::function<void(double)>& cb);
    ~NtpClient();

    int32_t SendTimeRequest();

private:
    int32_t InitNtpClient();
    int32_t HandleNtpBytes(const std::string& bytes);
    double  GetOffset(void* ntp, void* local);

private:
    bool mIsReady;
    uint16_t mPort;
    std::string mAddr;
    std::shared_ptr<PUdp> mNtpCliPtr;
    std::function<void(double)> mCb;
};

#endif // __NTP_CLIENT_H__
