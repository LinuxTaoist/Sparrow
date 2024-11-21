/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : NtpClient.cpp
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
#include <memory>
#include <cstring>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "SprLog.h"
#include "NtpClient.h"

using namespace std;

#define SPR_LOGD(fmt, args...) LOGD("NtpClient", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("NtpClient", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("NtpClient", fmt, ##args)

#define TIMEOUT             2
#define NTP_LI              0
#define NTP_VERSION_NUM     3
#define NTP_MODE_CLIENT     3
#define NTP_MODE_SERVER     4
#define NTP_STRATUM         0
#define NTP_POLL            4
#define NTP_PRECISION      -6
#define NTP_MIN_LEN        48
#define JAN_1970   0x83aa7e80

#define NTP_CONV_FRAC32(x) (uint64_t)((x) * ((uint64_t)1 << 32))
#define NTP_REVE_FRAC32(x) ((double)((double)(x) / ((uint64_t)1 << 32)))
#define NTP_CONV_FRAC16(x) (uint32_t)((x) * ((uint32_t)1 << 16))
#define NTP_REVE_FRAC16(x) ((double)((double)(x) / ((uint32_t)1 << 16)))
#define USEC2FRAC(x) ((uint32_t)NTP_CONV_FRAC32((x) / 1000000.0))
#define FRAC2USEC(x) ((uint32_t)NTP_REVE_FRAC32((x)*1000000.0))
#define NTP_LFIXED2DOUBLE(x) ((double)(ntohl(((struct LongFixedPt *)(x))->intPart) - JAN_1970 + FRAC2USEC(ntohl(((struct LongFixedPt *)(x))->fracPart)) / 1000000.0))

struct ShortFixedPt
{
    uint16_t intPart;
    uint16_t fracPart;
};

struct LongFixedPt
{
    uint32_t intPart;
    uint32_t fracPart;
};

struct NtpHdr
{
#if __BYTE_ORDER == __BIG_ENDIAN
    unsigned int ntpLi   : 2;
    unsigned int ntpVn   : 3;
    unsigned int ntpMode : 3;
#endif
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int ntpMode : 3;
    unsigned int ntpVn   : 3;
    unsigned int ntpLi   : 2;
#endif
    uint8_t ntpStratum;
    uint8_t ntpPoll;
    int8_t ntpPrecision;
    struct ShortFixedPt ntpRtDelay;
    struct ShortFixedPt ntpRtDispersion;
    uint32_t ntpRefId;
    struct LongFixedPt ntpRefTs;
    struct LongFixedPt ntpOriTs;
    struct LongFixedPt ntpRecvTs;
    struct LongFixedPt ntpTransTs;
};

NtpClient::NtpClient(const std::string addr, uint16_t port, const std::function<void(double)>& cb)
    : mIsReady(false), mPort(port), mAddr(addr), mCb(cb)
{
    InitNtpClient();
}

NtpClient::~NtpClient()
{
}

int32_t NtpClient::SendTimeRequest()
{
    int ret = -1;
    if (!mNtpCliPtr) {
        SPR_LOGE("mNtpCliPtr is nullptr\n");
        return ret;
    }

    if (!mIsReady) {
        SPR_LOGD("Creating tcp client %s:%d\n", mAddr.c_str(), mPort);
        ret = mNtpCliPtr->AsTcpClient(true, mAddr.c_str(), mPort);
        if (ret == -1) {
            SPR_LOGE("As tcp client %s:%d failed!\n", mAddr.c_str(), mPort);
            return ret;
        }
    }

    struct NtpHdr ntp;
    struct timeval tv;
    mIsReady = true;
    ntp.ntpLi = NTP_LI;
    ntp.ntpVn = NTP_VERSION_NUM;
    ntp.ntpMode = NTP_MODE_CLIENT;
    ntp.ntpStratum = NTP_STRATUM;
    ntp.ntpPoll = NTP_POLL;
    ntp.ntpPrecision = NTP_PRECISION;

    gettimeofday(&tv, nullptr);
    ntp.ntpTransTs.intPart = htonl(tv.tv_sec + JAN_1970);
    ntp.ntpTransTs.fracPart = htonl(USEC2FRAC(tv.tv_usec));

    if (mNtpCliPtr) {
        size_t size = sizeof(struct NtpHdr);
        std::string bytes(size, '\0');
        std::memcpy(&bytes[0], &ntp, size);
        ret = mNtpCliPtr->Write(mNtpCliPtr->GetEpollFd(), bytes);
    }

    return ret;
}

int32_t NtpClient::InitNtpClient()
{
    mNtpCliPtr = make_shared<PSocket>(AF_INET, SOCK_STREAM, 0, [&](int sock, void *arg) {
        PSocket* pCliObj = (PSocket*)arg;
        if (pCliObj == nullptr) {
            SPR_LOGE("PSocket is nullptr\n");
            return;
        }

        std::string rBuf;
        int rc = pCliObj->Read(sock, rBuf);
        if (rc > 0) {
            SPR_LOGD("# RECV [%d]> %d\n", sock, rBuf.size());
            HandleNtpBytes(rBuf);
        } else {
            SPR_LOGD("# CLOSE [%d]\n", sock);
        }

        // Only once read
        mIsReady = false;
        pCliObj->DelFromPoll();
        pCliObj->Close();
    });

    return mNtpCliPtr ? 0 : -1;
}

double NtpClient::GetOffset(void* ntp, void* local)
{
    if (ntp == nullptr || local == nullptr) {
        return 0.0;
    }

    double t1, t2, t3, t4;
    struct NtpHdr* pNtp = (struct NtpHdr*)ntp;
    struct timeval* pLocal = (struct timeval*)local;
    t1 = NTP_LFIXED2DOUBLE(&pNtp->ntpOriTs);
    t2 = NTP_LFIXED2DOUBLE(&pNtp->ntpRecvTs);
    t3 = NTP_LFIXED2DOUBLE(&pNtp->ntpTransTs);
    t4 = pLocal->tv_sec + pLocal->tv_usec / 1000000.0;
    return ((t2 - t1) + (t3 - t4)) / 2.0;
}

int32_t NtpClient::HandleNtpBytes(const std::string& bytes)
{
    struct NtpHdr ntp;
    std::memcpy(&ntp, &bytes[0], sizeof(struct NtpHdr));

    struct timeval tv;
    gettimeofday(&tv, nullptr);
    double offset = GetOffset(&ntp, &tv);
    tv.tv_sec += offset;
    if (mCb) {
        double time = 0;
        mCb(time);
    }

    return 0;
}
