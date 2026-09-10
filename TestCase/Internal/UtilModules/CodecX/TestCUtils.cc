/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestCUtils.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : CUtils 工具函数（CRC/LEB128/四则运算/文件读取）内部测试
 *  @date       : 2026/09/09
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/09 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <cstdio>
#include <fstream>
#include <vector>
#include <string>
#include "CUtils.h"
#include "gtest/gtest.h"

using namespace CUtils;

class UtilModules_CUtils : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {
        // 清理本套件所有 case 可能产生的临时文件，避免残留
        remove("/tmp/spr_cutils_readfile_test.txt");
        remove("/tmp/spr_cutils_hex_test.txt");
        remove("/tmp/spr_cutils_hex_empty.txt");
    }
};

// ---------- Crc8 / Crc16 / Bcc8 ----------
// 测试 CRC8 已知校验向量
TEST(UtilModules_CUtils, Crc8KnownVectors)
{
    // CRC-8/ATM (poly 0x07, init 0x00), "123456789" -> 0xF4
    const std::vector<uint8_t> data = {'1','2','3','4','5','6','7','8','9'};
    EXPECT_EQ(Crc8(data), 0xF4);

    const std::vector<uint8_t> empty;
    EXPECT_EQ(Crc8(empty), 0x00);
}

// 测试 CRC16 已知校验向量
TEST(UtilModules_CUtils, Crc16KnownVectors)
{
    // CRC-16/MODBUS (poly 0xA001, init 0xFFFF), "123456789" -> 0x4B37
    const std::vector<uint8_t> data = {'1','2','3','4','5','6','7','8','9'};
    EXPECT_EQ(Crc16(data), 0x4B37);

    const std::vector<uint8_t> empty;
    EXPECT_EQ(Crc16(empty), 0xFFFF);
}

// 测试 BCC8 异或累加
TEST(UtilModules_CUtils, Bcc8XorAccumulation)
{
    const std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04};
    EXPECT_EQ(Bcc8(data), 0x01 ^ 0x02 ^ 0x03 ^ 0x04);

    const std::vector<uint8_t> empty;
    EXPECT_EQ(Bcc8(empty), 0x00);
}

// ---------- DecodeLeb128 ----------
TEST(UtilModules_CUtils, DecodeLeb128SingleByte)
{
    // 测试单字节 LEB128 解码
    std::vector<uint8_t> bytes = {0x05};   // 0x05, no continuation
    int32_t offset = 0;
    EXPECT_EQ(DecodeLeb128(bytes, offset, 4), 5);
    EXPECT_EQ(offset, 1);
}

// 测试多字节 LEB128 解码
TEST(UtilModules_CUtils, DecodeLeb128MultiByte)
{
    std::vector<uint8_t> bytes = {0xE5, 0x8E, 0x26};   // 624485
    int32_t offset = 0;
    EXPECT_EQ(DecodeLeb128(bytes, offset, 4), 624485);
    EXPECT_EQ(offset, 3);
}

// 测试 LEB128 偏移越界返回失败
TEST(UtilModules_CUtils, DecodeLeb128OffsetOutOfRange)
{
    std::vector<uint8_t> bytes = {0x01, 0x02};
    int32_t offset = 5;   // >= size
    EXPECT_EQ(DecodeLeb128(bytes, offset, 4), -1);
    EXPECT_EQ(offset, 5);
}

// ---------- CalculateFromString ----------
// 测试四则运算基础表达式
TEST(UtilModules_CUtils, CalculateBasic)
{
    int32_t result = 0;
    EXPECT_EQ(CalculateFromString("1+2*3", result), 0);
    EXPECT_EQ(result, 7);

    EXPECT_EQ(CalculateFromString("(1+2)*3", result), 0);
    EXPECT_EQ(result, 9);

    EXPECT_EQ(CalculateFromString("10/2", result), 0);
    EXPECT_EQ(result, 5);

    EXPECT_EQ(CalculateFromString("-5+10", result), 0);
    EXPECT_EQ(result, 5);
}

// 测试四则运算错误处理（除零/括号/非法字符）
TEST(UtilModules_CUtils, CalculateErrors)
{
    int32_t result = 0;
    EXPECT_EQ(CalculateFromString("", result), -1);         // 空
    EXPECT_EQ(CalculateFromString("1/0", result), -3);      // 除零
    EXPECT_EQ(CalculateFromString("(1+2", result), -4);     // 括号不匹配
    EXPECT_EQ(CalculateFromString("1++2", result), -2);     // 无效字符
    EXPECT_EQ(CalculateFromString("1a2", result), -2);      // 非法字符
}

// ---------- ReadFile ----------
// 测试文件读取正常与文件不存在
TEST(UtilModules_CUtils, ReadFileNormalAndMissing)
{
    const std::string path = "/tmp/spr_cutils_readfile_test.txt";
    std::ofstream ofs(path, std::ios::binary);
    ofs << "hello-cutils";
    ofs.close();

    std::string content;
    int32_t ret = ReadFile(path, content);
    EXPECT_EQ(ret, static_cast<int32_t>(content.size()));
    EXPECT_EQ(content, "hello-cutils");

    EXPECT_EQ(ReadFile("/tmp/no_such_cutils_file.txt", content), -1);

    remove(path.c_str());
}

// ---------- ReadHexTextToHexVector ----------
// 测试十六进制文本转向量正常情况
TEST(UtilModules_CUtils, ReadHexTextNormal)
{
    const std::string path = "/tmp/spr_cutils_hex_test.txt";
    std::ofstream ofs(path, std::ios::binary);
    ofs << "123456AB";
    ofs.close();

    std::vector<uint8_t> out;
    ssize_t ret = ReadHexTextToHexVector(path, out);
    EXPECT_EQ(ret, 4);
    ASSERT_EQ(out.size(), 4u);
    EXPECT_EQ(out[0], 0x12);
    EXPECT_EQ(out[1], 0x34);
    EXPECT_EQ(out[2], 0x56);
    EXPECT_EQ(out[3], 0xAB);

    remove(path.c_str());
}

// 测试十六进制文本空文件与文件不存在
TEST(UtilModules_CUtils, ReadHexTextEmptyAndMissing)
{
    const std::string emptyPath = "/tmp/spr_cutils_hex_empty.txt";
    std::ofstream ofs(emptyPath, std::ios::binary);
    ofs.close();

    std::vector<uint8_t> out;
    EXPECT_EQ(ReadHexTextToHexVector(emptyPath, out), 0);
    EXPECT_EQ(ReadHexTextToHexVector("/tmp/no_such_hex_cutils.txt", out), -1);

    remove(emptyPath.c_str());
}
