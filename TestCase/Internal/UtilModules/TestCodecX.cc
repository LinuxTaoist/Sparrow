/*---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestCUtils.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/12/26
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/12/26 | 1.0.0.1   | Xiang.D        | Create file
 *  2026/01/04 | 1.0.0.2   | Xiang.D        | Adjust test cases for protocol codec scenario, remove padding logic
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <cstring>
#include <climits>
#include <gtest/gtest.h>
#include "CUtils.h"

using namespace std;
using namespace CUtils;

using SizeType = size_t;
using ReturnType = int32_t;

// --------------------------------------------------------------------------------------------------------------------
// - Util_SToI (string → 单个整数) - 协议编解码核心场景：字段读取、端序、溢出、部分字段
// --------------------------------------------------------------------------------------------------------------------
// 协议场景：大端序、1字节协议字段（uint8_t状态码）
TEST(UtilModules_CodecX, SToI_NormalBigEndianUint8ProtocolCase) {
    string str1 = "\x01"; // 协议中状态码0x01（成功）
    uint8_t out1 = 0;
    ReturnType ret = SToI(str1, out1, CENDIAN_BIG, 0);
    EXPECT_EQ(ret, ReturnType(1));
    EXPECT_EQ(out1, static_cast<uint8_t>(0x01));
}

// 协议场景：小端序、2字节协议字段（int16_t长度字段，负数表示异常）
TEST(UtilModules_CodecX, SToI_NormalLittleEndianInt16ProtocolCase) {
    string str2 = "\xFE\xFF"; // 小端-2（协议中长度异常）
    int16_t out2 = 0;
    ReturnType ret = SToI(str2, out2, CENDIAN_LITTLE, 0);
    EXPECT_EQ(ret, ReturnType(2));
    EXPECT_EQ(out2, static_cast<int16_t>(-2));
}

// 协议场景：大端序、4字节协议字段（uint32_t包序号）+ 非0偏移（跳过协议头2字节）
TEST(UtilModules_CodecX, SToI_NormalBigEndianUint32WithOffsetProtocolCase) {
    string str3({(int8_t)0xAA, 0x55, 0x00, 0x01, 0x02, 0x03}); // 协议头0xAA55 + 包序号0x00010203
    uint32_t out3 = 0;
    ReturnType ret = SToI(str3, out3, CENDIAN_BIG, 2);
    EXPECT_EQ(ret, ReturnType(4));
    EXPECT_EQ(out3, static_cast<uint32_t>(0x00010203));
}

// 协议场景：8字节协议字段（uint64_t时间戳）+ 偏移刚好匹配长度
TEST(UtilModules_CodecX, SToI_CriticalOffsetNormalProtocolCase) {
    string str4({0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, (int8_t)0x88}); // 偏移2，读取8字节时间戳
    uint64_t out4 = 0;
    ReturnType ret = SToI(str4, out4, CENDIAN_BIG, 1);
    EXPECT_EQ(ret, ReturnType(8));
    EXPECT_EQ(out4, static_cast<uint64_t>(0x1122334455667788));
}

// 协议异常：偏移为负数（非法协议解析偏移）
TEST(UtilModules_CodecX, SToI_NegativeOffsetErrorCase) {
    string str5 = "\x12\x34";
    uint16_t out5 = 0;
    ReturnType ret = SToI(str5, out5, CENDIAN_BIG, -1);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_EQ(out5, static_cast<uint16_t>(0));
}

// 协议场景：读取部分字段（协议中只取前1字节标识位）
TEST(UtilModules_CodecX, SToI_NormalReadPartialFieldProtocolCase) {
    string str6 = "\x12\x34\x56\x78"; // 4字节字段，仅读取前1字节标识位
    uint8_t out6 = 0;
    ReturnType ret = SToI(str6, out6, CENDIAN_BIG, 0, 1);
    EXPECT_EQ(ret, ReturnType(1));
    EXPECT_EQ(out6, static_cast<uint8_t>(0x12));
}

// 协议异常：空协议包
TEST(UtilModules_CodecX, SToI_EmptyStringErrorCase) {
    string str7 = "";
    uint8_t out7 = 0;
    ReturnType ret = SToI(str7, out7, CENDIAN_BIG, 0);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_EQ(out7, static_cast<uint8_t>(0));
}

// 协议异常：字段溢出（2字节数据转1字节字段，超出uint8_t范围）
TEST(UtilModules_CodecX, SToI_OverflowErrorProtocolCase) {
    string str8 = "\xFF\xFF"; // 2字节数据转uint8_t，溢出
    uint8_t out8 = 0;
    ReturnType ret = SToI(str8, out8, CENDIAN_BIG, 0);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_EQ(out8, static_cast<uint8_t>(0));
}

// 协议场景：有符号字段溢出（1字节0x80转int8_t，超出范围）
TEST(UtilModules_CodecX, SToI_SignedOverflowErrorProtocolCase) {
    string str9 = "\x80";
    int8_t out9 = 0;
    ReturnType ret = SToI(str9, out9, CENDIAN_BIG, 0);
    EXPECT_EQ(ret, ReturnType(1));
    EXPECT_EQ(out9, static_cast<int8_t>(0x80));
}

// 协议场景：指定len等于字段长度（4字节uint32_t协议字段）
TEST(UtilModules_CodecX, SToI_NormalLenEqualSizeProtocolCase) {
    string str10({0x11, 0x22, 0x33, 0x44}); // 4字节协议字段
    uint32_t out10 = 0;
    ReturnType ret = SToI(str10, out10, CENDIAN_BIG, 0, 4);
    EXPECT_EQ(ret, ReturnType(4));
    EXPECT_EQ(out10, static_cast<uint32_t>(0x11223344));
}

// 协议异常：len<0（非法参数）
TEST(UtilModules_CodecX, SToI_ErrorLenNegativeCase) {
    string str11 = "\x12";
    uint8_t out11 = 0;
    ReturnType ret = SToI(str11, out11, CENDIAN_BIG, 0, -1);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_EQ(out11, static_cast<uint8_t>(0));
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_SToV (string → 整数vector) - 协议编解码核心：批量字段、分包、偏移读取
// --------------------------------------------------------------------------------------------------------------------
// 协议场景：大端序、批量读取2字节uint16_t协议字段（多个状态码）
TEST(UtilModules_CodecX, SToV_NormalBigEndianUint16BatchProtocolCase) {
    string str1{0x00, 0x01, 0x00, 0x02, 0x00, 0x03}; // 3个2字节状态码字段
    vector<uint16_t> out1;
    ReturnType ret = SToV(str1, out1, CENDIAN_BIG, 0);
    EXPECT_EQ(ret, ReturnType(6));
    ASSERT_EQ(out1.size(), SizeType(3));
    EXPECT_EQ(out1[0], static_cast<uint16_t>(0x0001));
    EXPECT_EQ(out1[1], static_cast<uint16_t>(0x0002));
    EXPECT_EQ(out1[2], static_cast<uint16_t>(0x0003));
}

// 协议场景：追加模式（协议分包，多次读取合并）
TEST(UtilModules_CodecX, SToV_CoreAppendModeProtocolCase) {
    string str1 = {0x00, 0x01, 0x00, 0x02}; // 分包1：2个2字节字段
    string str2 = {0x00, 0x03, 0x00, 0x04}; // 分包2：2个2字节字段
    vector<uint16_t> out1;

    ReturnType ret1 = SToV(str1, out1, CENDIAN_BIG, 0);
    EXPECT_EQ(ret1, ReturnType(4));
    ASSERT_EQ(out1.size(), SizeType(2));
    EXPECT_EQ(out1[0], static_cast<uint16_t>(0x0001));
    EXPECT_EQ(out1[1], static_cast<uint16_t>(0x0002));

    ReturnType ret2 = SToV(str2, out1, CENDIAN_BIG, 0);
    EXPECT_EQ(ret2, ReturnType(4));
    ASSERT_EQ(out1.size(), SizeType(4));
    EXPECT_EQ(out1[2], static_cast<uint16_t>(0x0003));
    EXPECT_EQ(out1[3], static_cast<uint16_t>(0x0004));
}

// 协议场景：小端序、跳过协议头后读取int8_t批量字段
TEST(UtilModules_CodecX, SToV_NormalLittleEndianInt8WithOffsetProtocolCase) {
    string str2({(int8_t)0xAA, 0x55, 0x01, 0x02, 0x03, (int8_t)0xFF}); // 协议头0xAA55 + 4个int8_t字段
    vector<int8_t> out2;
    ReturnType ret = SToV(str2, out2, CENDIAN_LITTLE, 2);
    EXPECT_EQ(ret, ReturnType(4));
    ASSERT_EQ(out2.size(), SizeType(4));
    EXPECT_EQ(out2[0], static_cast<int8_t>(1));
    EXPECT_EQ(out2[1], static_cast<int8_t>(2));
    EXPECT_EQ(out2[2], static_cast<int8_t>(3));
    EXPECT_EQ(out2[3], static_cast<int8_t>(-1));
}

// 协议场景：剩余字段刚好匹配n*sizeof(T)（4字节uint32_t字段）
TEST(UtilModules_CodecX, SToV_CriticalRemainBytesNormalProtocolCase) {
    string str3 = "\x11\x22\x33\x44\x55\x66\x77\x88"; // 2个4字节uint32_t字段
    vector<uint32_t> out3;
    ReturnType ret = SToV(str3, out3, CENDIAN_BIG, 0);
    EXPECT_EQ(ret, ReturnType(8));
    ASSERT_EQ(out3.size(), SizeType(2));
    EXPECT_EQ(out3[0], static_cast<uint32_t>(0x11223344));
    EXPECT_EQ(out3[1], static_cast<uint32_t>(0x55667788));
}

// 协议异常：偏移为负数（非法协议头偏移）
TEST(UtilModules_CodecX, SToV_NegativeOffsetErrorCase) {
    string str4 = "\x12\x34";
    vector<uint16_t> out4;
    ReturnType ret = SToV(str4, out4, CENDIAN_BIG, -1);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_TRUE(out4.empty());
}

// 协议异常：剩余字节不足单个字段长度（协议包截断）
TEST(UtilModules_CodecX, SToV_InsufficientBytesErrorProtocolCase) {
    string str5 = "\x12"; // 2字节uint16_t字段，仅1字节数据
    vector<uint16_t> out5;
    ReturnType ret = SToV(str5, out5, CENDIAN_BIG, 0);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_TRUE(out5.empty());
}

// 协议异常：空协议包
TEST(UtilModules_CodecX, SToV_EmptyStringErrorCase) {
    string str6 = "";
    vector<int32_t> out6;
    ReturnType ret = SToV(str6, out6, CENDIAN_BIG, 0);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_TRUE(out6.empty());
}

// 协议场景：指定len读取部分批量字段（仅读取前2个int8_t字段）
TEST(UtilModules_CodecX, SToV_NormalLenSpecifiedProtocolCase) {
    string str7({(int8_t)0xAA, 0x55, 0x01, 0x02, 0x03, (int8_t)0xFF}); // 协议头+4个int8_t字段，仅读取前2个
    vector<int8_t> out7;
    ReturnType ret = SToV(str7, out7, CENDIAN_LITTLE, 2, 2);
    EXPECT_EQ(ret, ReturnType(2));
    ASSERT_EQ(out7.size(), SizeType(2));
    EXPECT_EQ(out7[0], static_cast<int8_t>(1));
    EXPECT_EQ(out7[1], static_cast<int8_t>(2));
}

// 协议异常：len<0（非法参数）
TEST(UtilModules_CodecX, SToV_ErrorLenNegativeCase) {
    string str9 = "\x11\x22\x33\x44";
    vector<uint16_t> out9;
    ReturnType ret = SToV(str9, out9, CENDIAN_BIG, 0, -1);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_TRUE(out9.empty());
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_IToS (单个整数 → string) - 协议编解码核心：字段序列化、端序、极值
// --------------------------------------------------------------------------------------------------------------------
// 协议场景：大端序、1字节状态码（uint8_t最大值0xFF）
TEST(UtilModules_CodecX, IToS_NormalBigEndianUint8MaxProtocolCase) {
    uint8_t in1 = 0xFF; // 协议中错误状态码
    string out1;
    ReturnType ret = IToS(in1, out1, CENDIAN_BIG);
    EXPECT_EQ(ret, ReturnType(1));
    ASSERT_EQ(out1.size(), SizeType(1));
    EXPECT_EQ(static_cast<uint8_t>(out1[0]), static_cast<uint8_t>(0xFF));
}

// 协议场景：小端序、2字节长度字段（int16_t最小值-32768）
TEST(UtilModules_CodecX, IToS_NormalLittleEndianInt16MinProtocolCase) {
    int16_t in2 = -32768; // 协议中长度字段最小值
    string out2;
    ReturnType ret = IToS(in2, out2, CENDIAN_LITTLE);
    EXPECT_EQ(ret, ReturnType(2));
    ASSERT_EQ(out2.size(), SizeType(2));
    EXPECT_EQ(static_cast<uint8_t>(out2[0]), static_cast<uint8_t>(0x00));
    EXPECT_EQ(static_cast<uint8_t>(out2[1]), static_cast<uint8_t>(0x80));
}

// 协议场景：大端序、8字节时间戳（uint64_t 0值）
TEST(UtilModules_CodecX, IToS_NormalBigEndianUint64ZeroProtocolCase) {
    uint64_t in3 = 0; // 协议中无效时间戳
    string out3;
    ReturnType ret = IToS(in3, out3, CENDIAN_BIG);
    EXPECT_EQ(ret, ReturnType(8));
    ASSERT_EQ(out3.size(), SizeType(8));
    for (char c : out3) {
        EXPECT_EQ(static_cast<uint8_t>(c), static_cast<uint8_t>(0x00));
    }
}

// 协议核心：编解码闭环（序列化→反序列化，协议字段一致性）
TEST(UtilModules_CodecX, IToS_CoreCodecLoopProtocolCase) {
    uint32_t in4 = 0x11223344; // 协议包序号
    string out4;
    ReturnType ret1 = IToS(in4, out4, CENDIAN_BIG);
    EXPECT_EQ(ret1, ReturnType(4));

    uint32_t in5 = 0;
    ReturnType ret2 = SToI(out4, in5, CENDIAN_BIG, 0);
    EXPECT_EQ(ret2, ReturnType(4));
    EXPECT_EQ(in5, static_cast<uint32_t>(0x11223344));
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_IToV (单个整数 → vector<U>) - 协议编解码核心：字段拆分为字节流、跨类型转换
// --------------------------------------------------------------------------------------------------------------------
// 协议场景：大端序、1字节状态码拆分为uint8_t vector
TEST(UtilModules_CodecX, IToV_NormalBigEndianUint8ToUint8MaxProtocolCase) {
    uint8_t in1 = 0xFF; // 协议状态码
    std::vector<uint8_t> out1;
    ReturnType ret = CUtils::IToV(in1, out1, CENDIAN_BIG);
    EXPECT_EQ(ret, ReturnType(1));
    ASSERT_EQ(out1.size(), SizeType(1));
    EXPECT_EQ(out1[0], static_cast<uint8_t>(0xFF));
}

// 协议场景：追加模式（协议字节流分段组装）
TEST(UtilModules_CodecX, IToV_CoreAppendModeProtocolCase) {
    uint16_t in1 = 0x0001; // 协议字段1
    uint16_t in2 = 0x0002; // 协议字段2
    vector<uint8_t> out1;

    ReturnType ret1 = CUtils::IToV(in1, out1, CENDIAN_BIG);
    EXPECT_EQ(ret1, ReturnType(2));
    ASSERT_EQ(out1.size(), SizeType(2));
    EXPECT_EQ(out1[0], static_cast<uint8_t>(0x00));
    EXPECT_EQ(out1[1], static_cast<uint8_t>(0x01));

    ReturnType ret2 = CUtils::IToV(in2, out1, CENDIAN_BIG);
    EXPECT_EQ(ret2, ReturnType(2));
    ASSERT_EQ(out1.size(), SizeType(4));
    EXPECT_EQ(out1[2], static_cast<uint8_t>(0x00));
    EXPECT_EQ(out1[3], static_cast<uint8_t>(0x02));
}

// 协议场景：小端序、2字节长度字段拆分为int16_t vector
TEST(UtilModules_CodecX, IToV_NormalLittleEndianInt16ToInt16MinProtocolCase) {
    int16_t in2 = -32768; // 协议长度字段
    std::vector<int16_t> out2;
    ReturnType ret = CUtils::IToV(in2, out2, CENDIAN_LITTLE);
    EXPECT_EQ(ret, ReturnType(2));
    ASSERT_EQ(out2.size(), SizeType(1));
    EXPECT_EQ(out2[0], static_cast<int16_t>(-32768));
}

// 协议场景：4字节uint32_t拆分为4个uint8_t（协议字节流）
TEST(UtilModules_CodecX, IToV_NormalBigEndianUint32ToUint8ProtocolCase) {
    uint32_t in7 = 0x11223344; // 协议包序号
    std::vector<uint8_t> out7;
    ReturnType ret = CUtils::IToV(in7, out7, CENDIAN_BIG);
    EXPECT_EQ(ret, ReturnType(4));
    ASSERT_EQ(out7.size(), SizeType(4));
    EXPECT_EQ(out7[0], static_cast<uint8_t>(0x11));
    EXPECT_EQ(out7[1], static_cast<uint8_t>(0x22));
    EXPECT_EQ(out7[2], static_cast<uint8_t>(0x33));
    EXPECT_EQ(out7[3], static_cast<uint8_t>(0x44));
}

// 协议异常：转换溢出（uint8_t 0xFF转int8_t，超出范围）
TEST(UtilModules_CodecX, IToV_OverflowErrorProtocolCase) {
    uint8_t in11 = 0x80; // 超出int8_t最大值0x7F
    std::vector<int8_t> out11;
    ReturnType ret = CUtils::IToV(in11, out11, CENDIAN_LITTLE);
    EXPECT_EQ(ret, ReturnType(1));
    ASSERT_EQ(out11.size(), SizeType(1));
    EXPECT_EQ(out11[0], static_cast<int8_t>(0x80));
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_VToI (字节vector → 单个整数) - 协议编解码核心：字节流解析为字段、端序、部分读取
// --------------------------------------------------------------------------------------------------------------------
// 协议场景：大端序、4字节协议字段（uint32_t包序号）
TEST(UtilModules_CodecX, VToI_NormalBigEndianUint8ToUint32ProtocolCase) {
    vector<uint8_t> in1 = {0x11, 0x22, 0x33, 0x44}; // 协议包序号字节流
    uint32_t out1 = 0;
    ReturnType ret = VToI(in1, out1, CENDIAN_BIG, 0);
    EXPECT_EQ(ret, ReturnType(4));
    EXPECT_EQ(out1, static_cast<uint32_t>(0x11223344));
}

// 协议场景：小端序、2字节协议字段（int16_t负数长度）
TEST(UtilModules_CodecX, VToI_NormalLittleEndianUint8ToInt16ProtocolCase) {
    vector<uint8_t> in2 = {0xFE, 0xFF}; // 小端-2（协议长度异常）
    int16_t out2 = 0;
    ReturnType ret = VToI(in2, out2, CENDIAN_LITTLE, 0);
    EXPECT_EQ(ret, ReturnType(2));
    EXPECT_EQ(out2, static_cast<int16_t>(-2));
}

// 协议场景：跳过协议头后读取4字节字段
TEST(UtilModules_CodecX, VToI_CriticalOffsetNormalProtocolCase) {
    vector<uint8_t> in3 = {0xAA, 0x55, 0x11, 0x22, 0x33, 0x44}; // 协议头0xAA55 + 4字节字段
    uint32_t out3 = 0;
    ReturnType ret = VToI(in3, out3, CENDIAN_BIG, 2);
    EXPECT_EQ(ret, ReturnType(4));
    EXPECT_EQ(out3, static_cast<uint32_t>(0x11223344));
}

// 协议异常：偏移为负数
TEST(UtilModules_CodecX, VToI_NegativeOffsetErrorCase) {
    vector<uint8_t> in4 = {0x12, 0x34};
    uint16_t out4 = 0;
    ReturnType ret = VToI(in4, out4, CENDIAN_BIG, -1);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_EQ(out4, static_cast<uint16_t>(0));
}

// 协议场景：读取部分字段（仅取前2字节作为uint16_t标识）
TEST(UtilModules_CodecX, VToI_NormalReadPartialFieldProtocolCase) {
    vector<uint8_t> in5 = {0x12, 0x34}; // 2字节标识字段转uint32_t，仅读取实际字节
    uint32_t out5 = 0;
    ReturnType ret = VToI(in5, out5, CENDIAN_BIG, 0);
    EXPECT_EQ(ret, ReturnType(2));
    EXPECT_EQ(out5, static_cast<uint32_t>(0x1234));
}

// 协议异常：空字节流
TEST(UtilModules_CodecX, VToI_EmptyVectorErrorCase) {
    vector<uint8_t> in6;
    uint8_t out6 = 0;
    ReturnType ret = VToI(in6, out6, CENDIAN_BIG, 0);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_EQ(out6, static_cast<uint8_t>(0));
}

// 协议场景：指定len等于字段长度
TEST(UtilModules_CodecX, VToI_NormalLenEqualSizeProtocolCase) {
    vector<uint8_t> in7 = {0x11, 0x22, 0x33, 0x44};
    uint32_t out7 = 0;
    ReturnType ret = VToI(in7, out7, CENDIAN_BIG, 0, 4);
    EXPECT_EQ(ret, ReturnType(4));
    EXPECT_EQ(out7, static_cast<uint32_t>(0x11223344));
}

// 协议异常：字段溢出（2字节0xFFFF转uint8_t）
TEST(UtilModules_CodecX, VToI_OverflowErrorProtocolCase) {
    vector<uint8_t> in8 = {0xFF, 0xFF}; // 2字节转uint8_t，溢出
    uint8_t out8 = 0;
    ReturnType ret = VToI(in8, out8, CENDIAN_BIG, 0);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_EQ(out8, static_cast<uint8_t>(0));
}

// 协议核心：编解码闭环（字节流序列化→反序列化）
TEST(UtilModules_CodecX, VToI_CoreCodecLoopProtocolCase) {
    uint32_t in9 = 0x11223344; // 协议字段
    vector<uint8_t> out9;
    ReturnType ret1 = CUtils::IToV(in9, out9, CENDIAN_BIG);
    EXPECT_EQ(ret1, ReturnType(4));

    uint32_t in10 = 0;
    ReturnType ret2 = VToI(out9, in10, CENDIAN_BIG, 0);
    EXPECT_EQ(ret2, ReturnType(4));
    EXPECT_EQ(in10, static_cast<uint32_t>(0x11223344));
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_VToS (整数vector → string) - 协议编解码核心：批量字段序列化、协议包组装
// --------------------------------------------------------------------------------------------------------------------
// 协议场景：大端序、批量uint16_t字段组装为协议包体
TEST(UtilModules_CodecX, VToS_NormalBigEndianUint16ToEmptyStringProtocolCase) {
    vector<uint16_t> in1 = {0x0001, 0x0002, 0x0003}; // 3个2字节协议字段
    string out1;
    ReturnType ret = VToS(in1, out1, CENDIAN_BIG, 0);
    EXPECT_EQ(ret, ReturnType(6));
    ASSERT_EQ(out1.size(), SizeType(6));
    EXPECT_EQ(static_cast<uint8_t>(out1[0]), static_cast<uint8_t>(0x00));
    EXPECT_EQ(static_cast<uint8_t>(out1[1]), static_cast<uint8_t>(0x01));
    EXPECT_EQ(static_cast<uint8_t>(out1[2]), static_cast<uint8_t>(0x00));
    EXPECT_EQ(static_cast<uint8_t>(out1[3]), static_cast<uint8_t>(0x02));
    EXPECT_EQ(static_cast<uint8_t>(out1[4]), static_cast<uint8_t>(0x00));
    EXPECT_EQ(static_cast<uint8_t>(out1[5]), static_cast<uint8_t>(0x03));
}

// 协议场景：追加模式（协议头+包体组装）
TEST(UtilModules_CodecX, VToS_CoreAppendModeProtocolCase) {
    vector<uint16_t> in1 = {0x0001, 0x0002}; // 包体字段1
    vector<uint16_t> in2 = {0x0003, 0x0004}; // 包体字段2
    string out1 = "HEAD_"; // 协议头

    ReturnType ret1 = VToS(in1, out1, CENDIAN_BIG, 0);
    EXPECT_EQ(ret1, ReturnType(4));
    ASSERT_EQ(out1.size(), SizeType(9));
    EXPECT_EQ(out1.substr(0, 5), "HEAD_");
    EXPECT_EQ(static_cast<uint8_t>(out1[5]), static_cast<uint8_t>(0x00));
    EXPECT_EQ(static_cast<uint8_t>(out1[6]), static_cast<uint8_t>(0x01));
    EXPECT_EQ(static_cast<uint8_t>(out1[7]), static_cast<uint8_t>(0x00));
    EXPECT_EQ(static_cast<uint8_t>(out1[8]), static_cast<uint8_t>(0x02));

    ReturnType ret2 = VToS(in2, out1, CENDIAN_BIG, 0);
    EXPECT_EQ(ret2, ReturnType(4));
    ASSERT_EQ(out1.size(), SizeType(13));
    EXPECT_EQ(static_cast<uint8_t>(out1[9]), static_cast<uint8_t>(0x00));
    EXPECT_EQ(static_cast<uint8_t>(out1[10]), static_cast<uint8_t>(0x03));
    EXPECT_EQ(static_cast<uint8_t>(out1[11]), static_cast<uint8_t>(0x00));
    EXPECT_EQ(static_cast<uint8_t>(out1[12]), static_cast<uint8_t>(0x04));
}

// 协议场景：小端序、int8_t字段写入协议包指定位置
TEST(UtilModules_CodecX, VToS_NormalLittleEndianInt8ToNonEmptyStringProtocolCase) {
    vector<int8_t> in2 = {1, 2, 3}; // int8_t数据字段
    string out2 = "PROTO"; // 协议头
    ReturnType ret = VToS(in2, out2, CENDIAN_LITTLE, 0, 0, 5); // 写入协议头后
    EXPECT_EQ(ret, ReturnType(3));
    ASSERT_EQ(out2.size(), SizeType(8));
    EXPECT_EQ(out2.substr(0, 5), "PROTO");
    EXPECT_EQ(static_cast<uint8_t>(out2[5]), static_cast<uint8_t>(0x01));
    EXPECT_EQ(static_cast<uint8_t>(out2[6]), static_cast<uint8_t>(0x02));
    EXPECT_EQ(static_cast<uint8_t>(out2[7]), static_cast<uint8_t>(0x03));
}

// 协议场景：读取协议字段字节流的部分内容（跳过1字节，读取2字节）
TEST(UtilModules_CodecX, VToS_CoreInputOffsetLenProtocolCase) {
    vector<uint32_t> in3 = {0x11223344}; // 4字节协议字段
    string out3;
    ReturnType ret = VToS(in3, out3, CENDIAN_BIG, 1, 2); // 跳过1字节，读取2字节
    EXPECT_EQ(ret, ReturnType(2));
    ASSERT_EQ(out3.size(), SizeType(2));
    EXPECT_EQ(static_cast<uint8_t>(out3[0]), static_cast<uint8_t>(0x22));
    EXPECT_EQ(static_cast<uint8_t>(out3[1]), static_cast<uint8_t>(0x33));
}

// 协议异常：输入偏移为负数
TEST(UtilModules_CodecX, VToS_NegativeInputOffsetErrorCase) {
    vector<int16_t> in5 = {0x1122};
    string out5 = "test";
    ReturnType ret = VToS(in5, out5, CENDIAN_BIG, -1);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_EQ(out5, static_cast<string>("test"));
}

// 协议异常：空vector（无包体数据）
TEST(UtilModules_CodecX, VToS_EmptyVectorNormalCase) {
    vector<int32_t> in6;
    string out6 = "hello";
    ReturnType ret = VToS(in6, out6, CENDIAN_BIG, 0);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_EQ(out6, static_cast<string>("hello"));
}

// 协议异常：输入len<0（非法参数）
TEST(UtilModules_CodecX, VToS_ErrorInputLenNegativeCase) {
    vector<uint16_t> in9 = {0x1122};
    string out9 = "test";
    ReturnType ret = VToS(in9, out9, CENDIAN_BIG, 0, -1);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_EQ(out9, static_cast<string>("test"));
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_VToV (整数vector → 整数vector) - 协议编解码核心：字节流转批量字段、跨类型转换
// --------------------------------------------------------------------------------------------------------------------
// 协议场景：大端序、字节流转2字节uint16_t字段
TEST(UtilModules_CodecX, VToV_NormalBigEndianUint8ToUint16ProtocolCase) {
    vector<uint8_t> in1 = {0x00, 0x01, 0x00, 0x02}; // 协议字节流
    vector<uint16_t> out1;
    ReturnType ret = VToV(in1, out1, CENDIAN_BIG, 0, 0);
    EXPECT_EQ(ret, ReturnType(4));
    ASSERT_EQ(out1.size(), SizeType(2));
    EXPECT_EQ(out1[0], static_cast<uint16_t>(0x0001));
    EXPECT_EQ(out1[1], static_cast<uint16_t>(0x0002));
}

// 协议场景：追加模式（分段字节流合并为批量字段）
TEST(UtilModules_CodecX, VToV_CoreAppendModeProtocolCase) {
    vector<uint8_t> in1 = {0x00, 0x01, 0x00, 0x02}; // 分段1
    vector<uint8_t> in2 = {0x00, 0x03, 0x00, 0x04}; // 分段2
    vector<uint16_t> out1;

    ReturnType ret1 = VToV(in1, out1, CENDIAN_BIG, 0, 0);
    EXPECT_EQ(ret1, ReturnType(4));
    ASSERT_EQ(out1.size(), SizeType(2));
    EXPECT_EQ(out1[0], static_cast<uint16_t>(0x0001));
    EXPECT_EQ(out1[1], static_cast<uint16_t>(0x0002));

    ReturnType ret2 = VToV(in2, out1, CENDIAN_BIG, 0, 0);
    EXPECT_EQ(ret2, ReturnType(4));
    ASSERT_EQ(out1.size(), SizeType(4));
    EXPECT_EQ(out1[2], static_cast<uint16_t>(0x0003));
    EXPECT_EQ(out1[3], static_cast<uint16_t>(0x0004));
}

// 协议场景：小端序、字节流转uint16_t字段
TEST(UtilModules_CodecX, VToV_NormalLittleEndianUint8ToUint16ProtocolCase) {
    vector<uint8_t> in2 = {0x01, 0x00, 0x02, 0x00}; // 小端字节流
    vector<uint16_t> out2;
    ReturnType ret = VToV(in2, out2, CENDIAN_LITTLE, 0, 0);
    EXPECT_EQ(ret, ReturnType(4));
    ASSERT_EQ(out2.size(), SizeType(2));
    EXPECT_EQ(out2[0], static_cast<uint16_t>(0x0001));
    EXPECT_EQ(out2[1], static_cast<uint16_t>(0x0002));
}

// 协议场景：4字节字节流转uint32_t字段
TEST(UtilModules_CodecX, VToV_NormalBigEndianUint8ToUint32ProtocolCase) {
    vector<uint8_t> in3 = {0x11, 0x22, 0x33, 0x44}; // 4字节协议字段
    vector<uint32_t> out3;
    ReturnType ret = VToV(in3, out3, CENDIAN_BIG, 0, 0);
    EXPECT_EQ(ret, ReturnType(4));
    ASSERT_EQ(out3.size(), SizeType(1));
    EXPECT_EQ(out3[0], static_cast<uint32_t>(0x11223344));
}

// 协议场景：跳过协议头后读取批量字段
TEST(UtilModules_CodecX, VToV_NormalNonZeroOffsetBatchProtocolCase) {
    vector<uint8_t> in6 = {0xAA, 0x55, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03}; // 协议头+3个2字节字段
    vector<uint16_t> out6;
    ReturnType ret = VToV(in6, out6, CENDIAN_BIG, 2, 0);
    EXPECT_EQ(ret, ReturnType(6));
    ASSERT_EQ(out6.size(), SizeType(3));
    EXPECT_EQ(out6[0], static_cast<uint16_t>(0x0001));
    EXPECT_EQ(out6[1], static_cast<uint16_t>(0x0002));
    EXPECT_EQ(out6[2], static_cast<uint16_t>(0x0003));
}

// 协议场景：指定len读取部分批量字段
TEST(UtilModules_CodecX, VToV_NormalSpecifiedInMaxLenProtocolCase) {
    vector<uint8_t> in7 = {0x00, 0x01, 0x00, 0x02, 0x00, 0x03}; // 3个2字节字段，仅读取前3字节
    vector<uint16_t> out7;
    ReturnType ret = VToV(in7, out7, CENDIAN_BIG, 0, 3);
    EXPECT_EQ(ret, ReturnType(3)); // 仅转换1个完整uint16_t字段
    ASSERT_EQ(out7.size(), SizeType(2));
    EXPECT_EQ(out7[0], static_cast<uint16_t>(0x0001));
    EXPECT_EQ(out7[1], static_cast<uint16_t>(0x0000));
}

// 协议异常：偏移为负数
TEST(UtilModules_CodecX, VToV_NegativeOffsetErrorCase) {
    vector<uint8_t> in8 = {0x11, 0x22};
    vector<uint16_t> out8;
    ReturnType ret = VToV(in8, out8, CENDIAN_BIG, -1, 0);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_TRUE(out8.empty());
}

// 协议异常：inMaxLen为负数
TEST(UtilModules_CodecX, VToV_NegativeInMaxLenErrorCase) {
    vector<uint8_t> in9 = {0x11, 0x22};
    vector<uint16_t> out9;
    ReturnType ret = VToV(in9, out9, CENDIAN_BIG, 0, -1);
    EXPECT_EQ(ret, ReturnType(-1));
    EXPECT_TRUE(out9.empty());
}

// 协议场景：有符号字段转换（int8_t负数流转int16_t）
TEST(UtilModules_CodecX, VToV_NormalInt8ToInt16SignedProtocolCase) {
    vector<int8_t> in11 = {-1, 1}; // 协议中int8_t状态字段
    vector<int16_t> out11;
    ReturnType ret = VToV(in11, out11, CENDIAN_BIG, 0, 0);
    EXPECT_EQ(ret, ReturnType(2));
    ASSERT_EQ(out11.size(), SizeType(1));
    EXPECT_EQ(out11[0], static_cast<int16_t>(0xFF01));
}

// 协议异常：空输入字节流
TEST(UtilModules_CodecX, VToV_EmptyInputVectorCase) {
    vector<uint8_t> in12;
    vector<uint16_t> out12;
    ReturnType ret = VToV(in12, out12, CENDIAN_BIG, 0, 0);
    EXPECT_EQ(ret, ReturnType(0));
    EXPECT_TRUE(out12.empty());
}

// 协议核心：编解码闭环（批量字段序列化→反序列化）
TEST(UtilModules_CodecX, VToV_CoreCodecLoopProtocolCase) {
    vector<uint32_t> in13 = {0x11223344, 0x55667788}; // 批量协议字段
    vector<uint8_t> out13;
    ReturnType ret1 = VToV(in13, out13, CENDIAN_BIG, 0, 0);
    EXPECT_EQ(ret1, ReturnType(8));

    uint32_t val1 = 0, val2 = 0;
    ReturnType ret2 = VToI(out13, val1, CENDIAN_BIG, 0, 4);
    ReturnType ret3 = VToI(out13, val2, CENDIAN_BIG, 4, 4);

    EXPECT_EQ(ret2, ReturnType(4));
    EXPECT_EQ(ret3, ReturnType(4));
    EXPECT_EQ(val1, static_cast<uint32_t>(0x11223344));
    EXPECT_EQ(val2, static_cast<uint32_t>(0x55667788));
}
