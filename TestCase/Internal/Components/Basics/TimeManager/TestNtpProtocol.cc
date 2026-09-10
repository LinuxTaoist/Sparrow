/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestNtpProtocol.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : NtpProtocol NTP 报文编解码内部测试
 *  @date       : 2026/09/10
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/10 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <string>
#include "NtpProtocol.h"
#include "gtest/gtest.h"

// 测试默认构造的字段默认值
TEST(UtilModules_NtpProtocol, DefaultConstruction)
{
    NtpProtocol proto("");

    EXPECT_EQ(proto.GetLi(), 0);
    EXPECT_EQ(proto.GetVn(), 4);
    EXPECT_EQ(proto.GetMode(), 3);
    EXPECT_EQ(proto.GetStratum(), 0);
    EXPECT_EQ(proto.GetPoll(), 4);
    EXPECT_EQ(proto.GetRootDelay(), 0u);
    EXPECT_EQ(proto.GetRootDispersion(), 0u);
    EXPECT_EQ(proto.GetReferenceId(), 0u);
}

// 测试所有字段 setter/getter 往返
TEST(UtilModules_NtpProtocol, SetGetRoundtrip)
{
    NtpProtocol proto("");

    proto.SetLi(2);
    proto.SetVn(4);
    proto.SetMode(5);
    proto.SetStratum(3);
    proto.SetPoll(10);
    proto.SetPrecision(-12);
    proto.SetRootDelay(0x12345678);
    proto.SetRootDispersion(0x9ABCDEF0);
    proto.SetReferenceId(0xDEADBEEF);
    proto.SetReferenceTimestamp(0x1111222233334444ULL);
    proto.SetReceiveTimestamp(0x5555666677778888ULL);
    proto.SetTransmitTimestamp(0x9999AAAABBBBCCCCULL);

    EXPECT_EQ(proto.GetLi(), 2);
    EXPECT_EQ(proto.GetVn(), 4);
    EXPECT_EQ(proto.GetMode(), 5);
    EXPECT_EQ(proto.GetStratum(), 3);
    EXPECT_EQ(proto.GetPoll(), 10);
    EXPECT_EQ(proto.GetPrecision(), 244);   // -12 作为 uint8_t
    EXPECT_EQ(proto.GetRootDelay(), 0x12345678u);
    EXPECT_EQ(proto.GetRootDispersion(), 0x9ABCDEF0u);
    EXPECT_EQ(proto.GetReferenceId(), 0xDEADBEEFu);
    EXPECT_EQ(proto.GetReferenceTimestamp(), 0x1111222233334444ULL);
    EXPECT_EQ(proto.GetReceiveTimestamp(), 0x5555666677778888ULL);
    EXPECT_EQ(proto.GetTransmitTimestamp(), 0x9999AAAABBBBCCCCULL);
}

// 测试认证密钥 ID 往返
TEST(UtilModules_NtpProtocol, AuthKeyIdRoundtrip)
{
    NtpProtocol proto("");
    proto.SetAuthKeyID("0123456789abcdef");

    std::string key;
    proto.GetAuthKeyID(key);
    EXPECT_EQ(key, "0123456789abcdef");
}

// 测试编码后再解码数据一致
TEST(UtilModules_NtpProtocol, EncodeDecodeRoundtrip)
{
    NtpProtocol proto("");
    proto.SetStratum(2);
    proto.SetPoll(6);
    proto.SetReferenceId(0x12345678);
    proto.SetReferenceTimestamp(0xAAAABBBBCCCCDDDDULL);
    proto.SetAuthKeyID("abcd");

    std::string encoded;
    int32_t size = proto.Encode(encoded);
    ASSERT_GT(size, 0);
    EXPECT_EQ(size, 48 + 4);   // 48 基础 + 4 字节认证密钥

    NtpProtocol decoded(encoded);
    EXPECT_EQ(decoded.GetStratum(), 2);
    EXPECT_EQ(decoded.GetPoll(), 6);
    EXPECT_EQ(decoded.GetReferenceId(), 0x12345678u);
    EXPECT_EQ(decoded.GetReferenceTimestamp(), 0xAAAABBBBCCCCDDDDULL);

    std::string key;
    decoded.GetAuthKeyID(key);
    EXPECT_EQ(key, "abcd");
}

// 测试解码过短报文返回失败
TEST(UtilModules_NtpProtocol, DecodeShortPacketFails)
{
    NtpProtocol proto("");
    EXPECT_EQ(proto.Decode("short"), -1);
}

// 测试编码非法字段返回失败
TEST(UtilModules_NtpProtocol, EncodeInvalidFieldFails)
{
    // VN 低于 3
    NtpProtocol p1("");
    p1.SetVn(2);
    std::string out;
    EXPECT_EQ(p1.Encode(out), -1);

    // Stratum 超过 16
    NtpProtocol p2("");
    p2.SetStratum(17);
    EXPECT_EQ(p2.Encode(out), -1);

    // Poll 超过 20
    NtpProtocol p3("");
    p3.SetPoll(21);
    EXPECT_EQ(p3.Encode(out), -1);
}

// 测试编码时更新时间戳
TEST(UtilModules_NtpProtocol, EncodeUpdatesOriginateTimestamp)
{
    NtpProtocol proto("");
    proto.SetOriginateTimestamp(0);

    std::string encoded;
    ASSERT_GT(proto.Encode(encoded), 0);

    // 编码时 UpdateOriginateTimestamp 会用当前时间覆盖
    EXPECT_GT(proto.GetOriginateTimestamp(), 0u);
}

// 测试无认证密钥时解码边界（恰好 48 字节）
TEST(UtilModules_NtpProtocol, DecodeExactlyBaseSize)
{
    NtpProtocol src("");
    src.SetStratum(5);

    std::string encoded;
    src.Encode(encoded);

    NtpProtocol decoded(encoded);
    std::string key;
    decoded.GetAuthKeyID(key);
    EXPECT_TRUE(key.empty());
}
