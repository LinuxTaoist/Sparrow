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
    : mArg(arg), mIsReady(false), mCb(cb), mLocalPort(port)
{
    InitSocket();
}

NtpSource::~NtpSource()
{
}

int32_t NtpSource::SendTimeRequest()
{
    for (auto& srv : mNtpServers) {
        SendTimeRequest(srv);
    }

    return 0;
}

int32_t NtpSource::InitSocket()
{
    mpSocket = make_shared<PUdp>([&](int sock, void* arg) {
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

int32_t NtpSource::SendTimeRequest(NtpServer& srv)
{
    int32_t ret = -1;
    if (!mIsReady) {
        SPR_LOGD("Creating UDP socket on port %d\n", mLocalPort);
        ret = mpSocket->AsUdp(mLocalPort);
        if (ret == -1 || !mpSocket) {
            SPR_LOGE("Create UDP failed! port %d \n", mLocalPort);
            return ret;
        }
        mIsReady = true;
    }

    std::string srvAddr = srv.addr;
    uint16_t srvPort = srv.port;
    std::string ip = SocketCommon::ResolveHostToIP(srvAddr);    // Warn: long time-consuming interface
    if (ip.empty()) {
        SPR_LOGE("Resolve host %s failed! (%s)\n", srvAddr.c_str(), strerror(errno));
        return -1;
    }

    std::string bytes;
    NtpProtocol ntpPacket("");
    ntpPacket.Encode(bytes);
    srv.ip = ip;
    srv.sendTs = GetCurTimeStampWithNtp();

    // mutiple thread request
    // if other thread request success, the socket will be closed
    if (mIsReady) {
        ret = mpSocket->Write(bytes, ip, srvPort);
        SPR_LOGD("Request to %s:%u %d bytes %s\n", ip.c_str(), srvPort, bytes.size(), ret == -1 ? "failed" : "success");
    }

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

    int64_t offsetNsec = 0;
    uint64_t t4 = GetCurTimeStampWithNtp();
    int32_t ret = GetOffsetNsec(ntpSrv->sendTs, ntpPacket.GetReceiveTimestamp(),
                    ntpPacket.GetTransmitTimestamp(), t4, offsetNsec);
    if (ret != 0) {
        SPR_LOGE("GetOffsetNsec failed!\n");
        return -1;
    }

    if (mCb) {
        mCb(offsetNsec, mArg);
    }

    return 0;
}

uint64_t NtpSource::GetCurTimeStampWithNtp()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        SPR_LOGE("Get orgin time failed! (%s)", strerror(errno));
        return 0;
    }

    uint64_t ntpSec = (uint64_t)ts.tv_sec + NTP_UNIX_EPOCH_OFFSET;
    uint64_t ntpFrac = ts.tv_nsec * 4294967296ULL / 1000000000ULL;
    return (ntpSec * 4294967296ULL) | ntpFrac;
}

int32_t NtpSource::GetOffsetNsec(uint64_t t1, uint64_t t2, uint64_t t3, uint64_t t4, int64_t& ns)
{
    #define NTPTIME_TO_NSEC(x) ( \
        (int64_t)(((x >> 32) & 0xFFFFFFFF) - NTP_UNIX_EPOCH_OFFSET) * 1000000000ULL + \
        (int64_t)(( (x & 0xFFFFFFFF) * 1000000000ULL ) / 4294967296ULL) \
    )

    uint32_t utc = (uint32_t)(t3 >> 32) & 0xFFFFFFFF;
    if (utc < NTP_TIMESTAMP_CHECK) {
        SPR_LOGE("Invalid NTP time %d\n", utc);
        return -1;
    }

    int64_t cliTranNs = NTPTIME_TO_NSEC(t1);
    int64_t srvRecvNs = NTPTIME_TO_NSEC(t2);
    int64_t srvTranNs = NTPTIME_TO_NSEC(t3);
    int64_t cliRecvNs = NTPTIME_TO_NSEC(t4);

    ns = ((srvRecvNs - cliTranNs) + (srvTranNs - cliRecvNs)) / 2;

    SPR_LOGD("t1: (%llu.%llu), t2: (%llu.%llu), t3: (%llu.%llu), t4: (%llu.%llu), offset: %lldns\n",
        cliTranNs / 1000000000, cliTranNs % 1000000000, srvRecvNs / 1000000000, srvRecvNs % 1000000000,
        srvTranNs / 1000000000, srvTranNs % 1000000000, cliRecvNs / 1000000000, cliRecvNs % 1000000000, ns);

    return 0;
}

