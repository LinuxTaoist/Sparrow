/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : NtpProtocol.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/11/11
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/11/11 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <algorithm>
#include "SprLog.h"
#include "NtpProtocol.h"

#define LOG_TAG "NtpProtocol"

// NTP协议核心常量定义（遵循RFC 5905）
constexpr size_t NTP_BASE_PACKET_SIZE = 48;    // 基础NTP数据包大小（无认证字段）
constexpr size_t NTP_TIMESTAMP_LEN = 8;        // NTP时间戳长度（64位）
constexpr uint8_t NTP_LI_MAX = 3;              // Leap Indicator最大有效值（0-3）
constexpr uint8_t NTP_VN_MIN = 3;              // 支持的最小版本号（NTPv3）
constexpr uint8_t NTP_VN_MAX = 4;              // 支持的最大版本号（NTPv4）
constexpr uint8_t NTP_MODE_MAX = 7;            // 模式字段最大有效值（0-7）
constexpr uint8_t NTP_STRATUM_MAX = 16;        // 层级字段最大有效值（0-16）
constexpr uint8_t NTP_POLL_MAX = 20;           // 轮询间隔最大有效值
constexpr uint64_t NTP_UNIX_EPOCH_OFFSET = 2208988800ULL; // NTP纪元与UNIX纪元的时间差（1970-1900）

NtpProtocol::NtpProtocol(const std::string& bytes)
{
    mLiVnMode.Li = 0;          // 无leap警告
    mLiVnMode.VN = 4;          // NTPv4
    mLiVnMode.Mode = 3;        // 客户端模式
    mStratum = 0;              // 未同步状态
    mPoll = 4;                 // 轮询间隔2^4=16秒
    mPrecision = -6;           // 精度2^-6=15.625ms
    mRootDelay = 0;
    mRootDispersion = 0;
    mReferenceId = 0;
    mReferenceTimestamp = 0;
    mOriginateTimestamp = 0;
    mReceiveTimestamp = 0;
    mTransmitTimestamp = 0;
    mAuthKeyIDs.clear();

    if (!bytes.empty()) {
        Decode(bytes);
    }
}

NtpProtocol::~NtpProtocol()
{
}

int32_t NtpProtocol::Encode(std::string& bytes)
{
    if (mLiVnMode.Li > NTP_LI_MAX ||
        mLiVnMode.VN < NTP_VN_MIN || mLiVnMode.VN > NTP_VN_MAX ||
        mLiVnMode.Mode > NTP_MODE_MAX ||
        mStratum > NTP_STRATUM_MAX ||
        mPoll > NTP_POLL_MAX) {
        return -1;
    }

    size_t totalSize = NTP_BASE_PACKET_SIZE + mAuthKeyIDs.size();
    bytes.resize(totalSize, 0);

    uint8_t* pBuf = reinterpret_cast<uint8_t*>(&bytes[0]);
    size_t offset = 0;

    pBuf[offset++] = (mLiVnMode.Li << 6) | (mLiVnMode.VN << 3) | mLiVnMode.Mode;
    pBuf[offset++] = mStratum;
    pBuf[offset++] = mPoll;
    pBuf[offset++] = mPrecision;

    *reinterpret_cast<uint32_t*>(pBuf + offset) = htonl(mRootDelay);
    offset += sizeof(uint32_t);

    *reinterpret_cast<uint32_t*>(pBuf + offset) = htonl(mRootDispersion);
    offset += sizeof(uint32_t);

    *reinterpret_cast<uint32_t*>(pBuf + offset) = htonl(mReferenceId);
    offset += sizeof(uint32_t);

    EncodeNtpTimestamp(mReferenceTimestamp, pBuf + offset);
    offset += NTP_TIMESTAMP_LEN;

    UpdateOriginateTimestamp();
    EncodeNtpTimestamp(mOriginateTimestamp, pBuf + offset);
    offset += NTP_TIMESTAMP_LEN;

    EncodeNtpTimestamp(mReceiveTimestamp, pBuf + offset);
    offset += NTP_TIMESTAMP_LEN;

    EncodeNtpTimestamp(mTransmitTimestamp, pBuf + offset);
    offset += NTP_TIMESTAMP_LEN;

    if (!mAuthKeyIDs.empty()) {
        memcpy(pBuf + offset, mAuthKeyIDs.data(), mAuthKeyIDs.size());
        offset += mAuthKeyIDs.size();
    }

    return 0;
}

int32_t NtpProtocol::Decode(const std::string& bytes)
{
    if (bytes.size() < NTP_BASE_PACKET_SIZE) {
        return -1;
    }

    const uint8_t* pBuf = reinterpret_cast<const uint8_t*>(bytes.data());
    size_t offset = 0;

    uint8_t liVnMode = pBuf[offset++];
    mLiVnMode.Li = (liVnMode >> 6) & 0x03;
    mLiVnMode.VN = (liVnMode >> 3) & 0x07;
    mLiVnMode.Mode = liVnMode & 0x07;

    mStratum = pBuf[offset++];
    mPoll = pBuf[offset++];
    mPrecision = static_cast<int8_t>(pBuf[offset++]);

    mRootDelay = ntohl(*reinterpret_cast<const uint32_t*>(pBuf + offset));
    offset += sizeof(uint32_t);

    mRootDispersion = ntohl(*reinterpret_cast<const uint32_t*>(pBuf + offset));
    offset += sizeof(uint32_t);

    mReferenceId = ntohl(*reinterpret_cast<const uint32_t*>(pBuf + offset));
    offset += sizeof(uint32_t);

    mReferenceTimestamp = DecodeNtpTimestamp(pBuf + offset);
    offset += NTP_TIMESTAMP_LEN;

    mOriginateTimestamp = DecodeNtpTimestamp(pBuf + offset);
    offset += NTP_TIMESTAMP_LEN;

    mReceiveTimestamp = DecodeNtpTimestamp(pBuf + offset);
    offset += NTP_TIMESTAMP_LEN;

    mTransmitTimestamp = DecodeNtpTimestamp(pBuf + offset);
    offset += NTP_TIMESTAMP_LEN;

    if (offset < bytes.size()) {
        mAuthKeyIDs.assign(pBuf + offset, pBuf + bytes.size());
    } else {
        mAuthKeyIDs.clear();
    }

    return 0;
}

void NtpProtocol::EncodeNtpTimestamp(uint64_t timestamp, uint8_t* pBuf)
{
    uint32_t sec = static_cast<uint32_t>((timestamp >> 32) & 0xFFFFFFFF);
    uint32_t frac = static_cast<uint32_t>(timestamp & 0xFFFFFFFF);

    *reinterpret_cast<uint32_t*>(pBuf) = htonl(sec);
    *reinterpret_cast<uint32_t*>(pBuf + 4) = htonl(frac);
}

uint64_t NtpProtocol::NtpProtocol::DecodeNtpTimestamp(const uint8_t* pBuf)
{
    uint32_t sec = ntohl(*reinterpret_cast<const uint32_t*>(pBuf));
    uint32_t frac = ntohl(*reinterpret_cast<const uint32_t*>(pBuf + 4));

    return (static_cast<uint64_t>(sec) << 32) | frac;
}

void NtpProtocol::UpdateOriginateTimestamp()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        mOriginateTimestamp = 0;
        SPR_LOGE("Get orgin time failed! (%s)", strerror(errno));
        return;
    }

    uint64_t ntpSec = (uint64_t)ts.tv_sec + NTP_UNIX_EPOCH_OFFSET;
    uint64_t ntpFrac = ts.tv_nsec * 4294967296ULL / 1000000000ULL;
    mOriginateTimestamp = (ntpSec << 32) | ntpFrac;
}

uint8_t NtpProtocol::GetLi()
{
    return mLiVnMode.Li;
}

uint8_t NtpProtocol::GetVn()
{
    return mLiVnMode.VN;
}

uint8_t NtpProtocol::GetMode()
{
    return mLiVnMode.Mode;
}

uint8_t NtpProtocol::GetStratum()
{
    return mStratum;
}

uint8_t NtpProtocol::GetPoll()
{
    return mPoll;
}

uint8_t NtpProtocol::GetPrecision()
{
    return mPrecision;
}

uint32_t NtpProtocol::GetRootDelay()
{
    return mRootDelay;
}

uint32_t NtpProtocol::GetRootDispersion()
{
    return mRootDispersion;
}

uint32_t NtpProtocol::GetReferenceId()
{
    return mReferenceId;
}

uint64_t NtpProtocol::GetReferenceTimestamp()
{
    return mReferenceTimestamp;
}

uint64_t NtpProtocol::GetOriginateTimestamp()
{
    return mOriginateTimestamp;
}

uint64_t NtpProtocol::GetReceiveTimestamp()
{
    return mReceiveTimestamp;
}

uint64_t NtpProtocol::GetTransmitTimestamp()
{
    return mTransmitTimestamp;
}

void NtpProtocol::GetAuthKeyID(std::string& key)
{
    key = mAuthKeyIDs;
}

void NtpProtocol::SetLi(uint8_t li)
{
    mLiVnMode.Li = li;
}

void NtpProtocol::SetVn(uint8_t vn)
{
    mLiVnMode.VN = vn;
}

void NtpProtocol::SetMode(uint8_t mode)
{
    mLiVnMode.Mode = mode;
}

void NtpProtocol::SetStratum(uint8_t stratum)
{
    mStratum = stratum;
}

void NtpProtocol::SetPoll(uint8_t poll)
{
    mPoll = poll;
}

void NtpProtocol::SetPrecision(uint8_t precision)
{
    mPrecision = precision;
}

void NtpProtocol::SetRootDelay(uint32_t rootDelay)
{
    mRootDelay = rootDelay;
}

void NtpProtocol::SetRootDispersion(uint32_t rootDispersion)
{
    mRootDispersion = rootDispersion;
}

void NtpProtocol::SetReferenceId(uint32_t referenceId)
{
    mReferenceId = referenceId;
}

void NtpProtocol::SetReferenceTimestamp(uint64_t referenceTimestamp)
{
    mReferenceTimestamp = referenceTimestamp;
}

void NtpProtocol::SetOriginateTimestamp(uint64_t originateTimestamp)
{
    mOriginateTimestamp = originateTimestamp;
}

void NtpProtocol::SetReceiveTimestamp(uint64_t receiveTimestamp)
{
    mReceiveTimestamp = receiveTimestamp;
}

void NtpProtocol::SetTransmitTimestamp(uint64_t transmitTimestamp)
{
    mTransmitTimestamp = transmitTimestamp;
}

void NtpProtocol::SetAuthKeyID(const std::string& key)
{
    mAuthKeyIDs = key;
}

void NtpProtocol::DumpDetails()
{
    SPR_LOGD("NTP Protocol Details:\n");
    SPR_LOGD("Li: %d, VN: %d, Mode: %d\n", mLiVnMode.Li, mLiVnMode.VN, mLiVnMode.Mode);
    SPR_LOGD("Stratum: %d, Poll: %d, Precision: %d\n", mStratum, mPoll, mPrecision);
    SPR_LOGD("Root Delay: %u, Root Dispersion: %u\n", mRootDelay, mRootDispersion);
    SPR_LOGD("Reference ID: %u\n", mReferenceId);
    SPR_LOGD("Reference Timestamp: %llu.%llu\n", ((mReferenceTimestamp >> 32) & 0xFFFFFFFF) - NTP_UNIX_EPOCH_OFFSET, mReferenceTimestamp & 0xFFFFFFFF);
    SPR_LOGD("Originate Timestamp: %llu.%llu\n", ((mOriginateTimestamp >> 32) & 0xFFFFFFFF), mOriginateTimestamp & 0xFFFFFFFF);
    SPR_LOGD("Receive Timestamp  : %llu.%llu\n", ((mReceiveTimestamp >> 32) & 0xFFFFFFFF) - NTP_UNIX_EPOCH_OFFSET, mReceiveTimestamp & 0xFFFFFFFF);
    SPR_LOGD("Transmit Timestamp : %llu.%llu\n", ((mTransmitTimestamp >> 32) & 0xFFFFFFFF) - NTP_UNIX_EPOCH_OFFSET, mTransmitTimestamp & 0xFFFFFFFF);
}