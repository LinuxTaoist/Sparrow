/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : NtpProtocol.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/11/11
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/11/11 | 1.0.0.1   | Xiang.D        | Create file
 *  2024/11/22 | 1.0.0.2   | Fixed          | 添加辅助函数声明，修正宏定义错误
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __NTP_PROTOCOL_H__
#define __NTP_PROTOCOL_H__

#include <string>
#include <stdint.h>

class NtpProtocol
{
public:
    NtpProtocol(const std::string& bytes);
    ~NtpProtocol();

    int32_t Encode(std::string& bytes);
    int32_t Decode(const std::string& bytes);

    uint8_t GetLi();
    uint8_t GetVn();
    uint8_t GetMode();
    uint8_t GetStratum();
    uint8_t GetPoll();
    uint8_t GetPrecision();
    uint32_t GetRootDelay();
    uint32_t GetRootDispersion();
    uint32_t GetReferenceId();
    uint64_t GetReferenceTimestamp();
    uint64_t GetOriginateTimestamp();
    uint64_t GetReceiveTimestamp();
    uint64_t GetTransmitTimestamp();
    void GetAuthKeyID(std::string& key);

    void SetLi(uint8_t li);
    void SetVn(uint8_t vn);
    void SetMode(uint8_t mode);
    void SetStratum(uint8_t stratum);
    void SetPoll(uint8_t poll);
    void SetPrecision(uint8_t precision);
    void SetRootDelay(uint32_t rootDelay);
    void SetRootDispersion(uint32_t rootDispersion);
    void SetReferenceId(uint32_t referenceId);
    void SetReferenceTimestamp(uint64_t referenceTimestamp);
    void SetOriginateTimestamp(uint64_t originateTimestamp);
    void SetReceiveTimestamp(uint64_t receiveTimestamp);
    void SetTransmitTimestamp(uint64_t transmitTimestamp);
    void SetAuthKeyID(const std::string& key);
    void DumpDetails();

private:
    void UpdateOriginateTimestamp();
    void EncodeNtpTimestamp(uint64_t timestamp, uint8_t* pBuf);
    uint64_t DecodeNtpTimestamp(const uint8_t* pBuf);

    struct LiVnMode
    {
        uint8_t Li : 2;
        uint8_t VN : 3;
        uint8_t Mode : 3;
    };

    LiVnMode mLiVnMode;
    uint8_t mStratum;
    uint8_t mPoll;
    uint8_t mPrecision;
    uint32_t mRootDelay;
    uint32_t mRootDispersion;
    uint32_t mReferenceId;
    uint64_t mReferenceTimestamp;
    uint64_t mOriginateTimestamp;
    uint64_t mReceiveTimestamp;
    uint64_t mTransmitTimestamp;
    std::string mAuthKeyIDs;
};

#endif // __NTP_PROTOCOL_H__
