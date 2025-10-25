/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : NtpSource.h
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
#ifndef __NTP_SOURCE_H__
#define __NTP_SOURCE_H__

#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <functional>
#include <stdint.h>
#include "PSocket.h"

class NtpSource {
public:
    using TimeCallback = std::function<void(uint64_t, void*)>;
    NtpSource(uint16_t port, const TimeCallback& cb, void* arg = nullptr);
    ~NtpSource();

    int32_t SendTimeRequest();
    int32_t AddNtpServer(const std::string& addr, uint16_t port = 123);

private:
    struct NtpServer {
        std::string addr;
        std::string ip;
        uint16_t port;
        uint64_t sendTs;
    };

    int32_t InitSocket();
    int32_t SendTimeRequest(NtpServer& srv);
    int32_t HandleNtpBytes(const std::string& bytes, const std::string& srcAddr);
    uint64_t GetCurTimeStamp();
    uint64_t CalculateTime(uint64_t t1, uint64_t t2, uint64_t t3, uint64_t t4);

private:
    void* mArg;                         // 回调函数参数
    std::atomic<bool> mIsReady;         // Socket是否准备就绪
    TimeCallback mCb;                   // 时间回调函数
    uint16_t mLocalPort;                // 本地端口
    std::shared_ptr<PUdp> mpSocket;     // UDP套接字
    std::vector<NtpServer> mNtpServers; // NTP服务器列表
};

#endif // __NTP_SOURCE_H__
