/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : NtpSource.cpp
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
#include <algorithm>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "SprLog.h"
#include "NtpProtocol.h"
#include "NtpSource.h"

using namespace std;

#define LOG_TAG "NtpSource"

#define NTP_TIMESTAMP_CHECK     1756403275ULL   // 2025-08-29 01:47:55
#define NTP_UNIX_EPOCH_OFFSET   2208988800ULL   // 1970-1900

NtpSource::NtpSource(uint16_t port, const TimeCallback& cb, void* arg)
    : mIsReady(false), mArg(arg), mCb(cb), mLocalPort(port), mCurSrvIndex(0)
{
    InitSocket();
}

NtpSource::~NtpSource()
{
}

int32_t NtpSource::SendTimeRequest()
{
    int32_t ret = -1;
    if (!mIsReady) {
        SPR_LOGD("Creating UDP socket on port %d\n", mLocalPort);
        ret = mpSocket->AsUdp(mLocalPort);
        if (ret == -1) {
            SPR_LOGE("Failed to create UDP socket on port %d\n", mLocalPort);
            return ret;
        }
        mIsReady = true;
    }

    if (!mpSocket) {
        SPR_LOGE("mpSocket is nullptr\n");
        return -1;
    }

    SPR_LOGD("Sending time request to %d NTP servers\n", mNtpServers.size());
    if (mCurSrvIndex >= (int32_t)mNtpServers.size()) {
        mCurSrvIndex = 0;
        mIsReady = false;
        mpSocket->Close();
        SPR_LOGW("All NTP servers have been tried, sync failed!\n");
        return -1;
    }

    std::string srvAddr = mNtpServers[mCurSrvIndex].addr;
    uint16_t srvPort = mNtpServers[mCurSrvIndex].port;
    std::string ip = SocketCommon::ResolveHostToIP(srvAddr);    // Warn: long time-consuming interface
    SPR_LOGD("[%d/%u] Resolve %s to %s\n", mCurSrvIndex + 1, mNtpServers.size(), srvAddr.c_str(), ip.c_str());

    if (!ip.empty()) {
        std::string bytes;
        NtpProtocol ntpPacket("");

        ntpPacket.Encode(bytes);
        mNtpServers[mCurSrvIndex].ip = ip;
        mNtpServers[mCurSrvIndex].sendTs = GetCurTimeStamp();
        ret = mpSocket->Write(bytes, ip, srvPort);
        SPR_LOGD("[%d/%u] Request to %s:%u %d bytes %s\n", mCurSrvIndex + 1, mNtpServers.size(),
                ip.c_str(), srvPort, bytes.size(), ret == -1 ? "failed" : "success");
    } else {
        SPR_LOGE("Resolve host %s failed! (%s)\n", srvAddr.c_str(), strerror(errno));
    }

    mCurSrvIndex++;
    return 0;
}

int32_t NtpSource::InitSocket()
{
    mpSocket = make_shared<PUdp>([&](int sock, void *arg) {
        PUdp* pUdp = static_cast<PUdp*>(arg);
        if (!pUdp) {
            SPR_LOGE("pUdp is nullptr\n");
            return;
        }

        uint16_t port = 0;
        std::string addr, rBuf;
        int32_t ret = pUdp->Read(rBuf, addr, port);
        if (ret <= 0) {
            SPR_LOGE("Failed to read from socket\n");
            return;
        }

        SPR_LOGD("Received %d bytes from %s:%u\n", ret, addr.c_str(), port);
        ret = HandleNtpBytes(rBuf, addr);
        if (ret == 0) {
            mIsReady = false;
            pUdp->Close();
        }
    });

    return mpSocket ? 0 : -1;
}

int32_t NtpSource::AddNtpServer(const std::string& addr, uint16_t port)
{
    if (addr.empty()) {
        SPR_LOGE("Invalid NTP server address\n");
        return -1;
    }

    uint16_t serverPort = (port > 0) ? port : 123;
    mNtpServers.push_back({addr, "", serverPort, 0});
    SPR_LOGD("Added NTP server: %s:%u\n", addr.c_str(), serverPort);
    return 0;
}

int32_t NtpSource::HandleNtpBytes(const std::string& bytes, const std::string& srcAddr)
{
    NtpProtocol ntpPacket(bytes);
    // ntpPacket.DumpDetails();

    auto ntpSrv = std::find_if(mNtpServers.begin(), mNtpServers.end(), [&srcAddr](const NtpServer& srv) {
        return srv.ip == srcAddr;
    });

    if (ntpSrv == mNtpServers.end()) {
        SPR_LOGE("Received from unknown NTP server %s\n", srcAddr.c_str());
        return -1;
    }

    uint64_t t4 = GetCurTimeStamp();
    uint64_t syncTime = CalculateTime(ntpSrv->sendTs, ntpPacket.GetTransmitTimestamp(),
                                      ntpPacket.GetReceiveTimestamp(), t4);

    uint32_t sec = (syncTime >> 32) & 0xFFFFFFFF;
    if (sec < NTP_TIMESTAMP_CHECK) {
        SPR_LOGE("Invalid NTP time %u\n", sec);
        return -1;
    }

    if (mCb) {
        mCb(syncTime, mArg);
    }

    // Has received normal response from NTP server
    // reset the index to 0
    mCurSrvIndex = 0;
    return 0;
}

uint64_t NtpSource::GetCurTimeStamp()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        SPR_LOGE("Get orgin time failed! (%s)", strerror(errno));
        return 0;
    }

    uint64_t ntpSec = (uint64_t)ts.tv_sec;
    uint64_t ntpFrac = ts.tv_nsec * 4294967296ULL / 1000000000ULL;
    return (ntpSec << 32) | ntpFrac;
}

uint64_t NtpSource::CalculateTime(uint64_t t1, uint64_t t2, uint64_t t3, uint64_t t4)
{
    uint32_t utc = ((t3 >> 32) & 0xFFFFFFFF) - NTP_UNIX_EPOCH_OFFSET;
    uint64_t offset = ((t4 - t1) + (t3 - t2)) / 2;
    uint32_t offsetSec = (offset >> 32) & 0xFFFFFFFF;
    uint32_t offsetFrac = offset & 0xFFFFFFFF;

    return (static_cast<uint64_t>(utc + offsetSec) << 32) | offsetFrac;
}
