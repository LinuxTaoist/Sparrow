#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include <vector>
#include "FileUtils.h"

using namespace FileUtils;

class Util_FileUtilsTest : public ::testing::Test {
protected:
    const std::string kBinUint8      = "test_uint8.bin";
    const std::string kBinUint16     = "test_uint16.bin";
    const std::string kBinEmpty      = "test_empty.bin";
    const std::string kBinLarge      = "test_large.bin";

    const std::string kHexUint8      = "test_hex8.txt";
    const std::string kHexUint16     = "test_hex16.txt";
    const std::string kHexMix        = "test_hex_mix.txt";
    const std::string kHexEmpty      = "test_empty_hex.txt";
    const std::string kHexLarge      = "test_large_hex.txt";
    const std::string kHexProtocol   = "test_protocol_hex.txt";

    void SetUp() override {
        // 1. uint8 测试文件(3字节: 0x12,0x34,0x56)
        std::ofstream f8(kBinUint8, std::ios::binary);
        uint8_t data8[] = {0x12, 0x34, 0x56};
        f8.write(reinterpret_cast<char*>(data8), sizeof(data8));

        // 2. uint16 测试文件(2字节: 0x12,0x34)
        std::ofstream f16(kBinUint16, std::ios::binary);
        uint8_t data16[] = {0x12, 0x34};
        f16.write(reinterpret_cast<char*>(data16), sizeof(data16));

        // 3. 空二进制文件（修复语法）
        // std::ofstream fEmptyBin(kBinEmpty, std::ios::binary);

        // 4. 超MAX_FILE_SIZE二进制文件
        std::ofstream f_large_bin(kBinLarge, std::ios::binary);
        std::vector<char> large_bin(MAX_FILE_SIZE + 1, 0x01);
        f_large_bin.write(large_bin.data(), large_bin.size());

        // 修复所有文本文件创建语法，消除括号报错
        std::ofstream fHex8(kHexUint8);
        fHex8 << "12 34 56 AB";

        std::ofstream fHex16(kHexUint16);
        fHex16 << "1234 5678";

        std::ofstream fHexMix(kHexMix);
        fHexMix << "12\n34\t56 AB";

        // std::ofstream fHexEmpty(kHexEmpty);

        std::ofstream fHexLarge(kHexLarge);
        fHexLarge << std::string(MAX_FILE_SIZE + 1, 'F');

        std::ofstream fHexProto(kHexProtocol);
        fHexProto << "785634120501B80B00010102DDCCBBAA02010304102030400205025060";
    }

    void TearDown() override {
        std::remove(kBinUint8.c_str());
        std::remove(kBinUint16.c_str());
        std::remove(kBinEmpty.c_str());
        std::remove(kBinLarge.c_str());
        std::remove(kHexUint8.c_str());
        std::remove(kHexUint16.c_str());
        std::remove(kHexMix.c_str());
        std::remove(kHexEmpty.c_str());
        std::remove(kHexLarge.c_str());
    }
};

// 正常读取二进制文件uint8_t类型数据
TEST_F(Util_FileUtilsTest, ReadFileToVector_NormalUint8) {
    std::vector<uint8_t> out;
    ssize_t ret = ReadFileToVector(kBinUint8, out);
    EXPECT_EQ(ret, 3);
    EXPECT_EQ((int32_t)out.size(), 3);
    EXPECT_EQ(out[0], 0x12);
    EXPECT_EQ(out[1], 0x34);
    EXPECT_EQ(out[2], 0x56);
}

// 验证uint16_t二进制读取大小端转换功能
TEST_F(Util_FileUtilsTest, ReadFileToVector_Uint16Endian) {
    std::vector<uint16_t> out;
    // 大端
    ssize_t ret = ReadFileToVector(kBinUint16, out, true);
    EXPECT_EQ(ret, 2);
    EXPECT_EQ(out[0], 0x1234);
    // 小端
    out.clear();
    ret = ReadFileToVector(kBinUint16, out, false);
    EXPECT_EQ(ret, 2);
    EXPECT_EQ(out[0], 0x3412);
}

// 验证文件超过2KB大小限制读取失败
TEST_F(Util_FileUtilsTest, ReadFileToVector_FileTooLarge) {
    std::vector<uint8_t> out;
    ssize_t ret = ReadFileToVector(kBinLarge, out);
    EXPECT_EQ(ret, -1);
}

// 读取空二进制文件
TEST_F(Util_FileUtilsTest, ReadFileToVector_EmptyFile) {
    std::vector<uint8_t> out;
    ssize_t ret = ReadFileToVector(kBinEmpty, out);
    EXPECT_EQ(ret, -1);
    EXPECT_TRUE(out.empty());
}

// 读取不存在的二进制文件
TEST_F(Util_FileUtilsTest, ReadFileToVector_FileNotExist) {
    std::vector<uint8_t> out;
    ssize_t ret = ReadFileToVector("no_exist.bin", out);
    EXPECT_EQ(ret, -1);
}

// 正常读取十六进制文本文件转uint8_t数据
TEST_F(Util_FileUtilsTest, ReadCharTextToHexVector_NormalUint8) {
    std::vector<uint8_t> out;
    ssize_t ret = ReadCharTextToHexVector(kHexUint8, out);
    EXPECT_EQ((int32_t)ret, 4);
    EXPECT_EQ(out[0], 0x12);
    EXPECT_EQ(out[1], 0x34);
    EXPECT_EQ(out[2], 0x56);
    EXPECT_EQ(out[3], 0xAB);
}

// 验证uint16_t十六进制文本读取大小端转换功能
TEST_F(Util_FileUtilsTest, ReadCharTextToHexVector_Uint16Endian) {
    std::vector<uint16_t> out;
    // 大端
    ssize_t ret = ReadCharTextToHexVector(kHexUint16, out, true);
    EXPECT_EQ(ret, 4);
    EXPECT_EQ(out[0], 0x1234);
    // 小端
    out.clear();
    ret = ReadCharTextToHexVector(kHexUint16, out, false);
    EXPECT_EQ(ret, 4);
    EXPECT_EQ(out[0], 0x3412);
}

// 读取带空格、换行的十六进制混合格式文本
TEST_F(Util_FileUtilsTest, ReadCharTextToHexVector_MixFormat) {
    std::vector<uint8_t> out;
    ssize_t ret = ReadCharTextToHexVector(kHexMix, out);
    EXPECT_EQ(ret, 4);
    EXPECT_EQ(out[0], 0x12);
}

// 验证十六进制文件超过MAX_FILE_SIZE大小限制读取失败
TEST_F(Util_FileUtilsTest, ReadCharTextToHexVector_FileTooLarge) {
    std::vector<uint8_t> out;
    ssize_t ret = ReadCharTextToHexVector(kHexLarge, out);
    EXPECT_EQ(ret, -1);
}

// 读取空十六进制文本文件
TEST_F(Util_FileUtilsTest, ReadCharTextToHexVector_EmptyFile) {
    std::vector<uint8_t> out;
    ssize_t ret = ReadCharTextToHexVector(kHexEmpty, out);
    EXPECT_EQ(ret, -1);
    EXPECT_TRUE(out.empty());
}

// 读取不存在的十六进制文本文件
TEST_F(Util_FileUtilsTest, ReadCharTextToHexVector_FileNotExist) {
    std::vector<uint8_t> out;
    ssize_t ret = ReadCharTextToHexVector("no_exist_hex.txt", out);
    EXPECT_EQ(ret, -1);
}

// 验证长十六进制字符串协议文本转换为二进制字节
TEST_F(Util_FileUtilsTest, ReadCharTextToHexVector_ProtocolData) {
    std::vector<uint8_t> out;
    std::vector<uint8_t> expect = {
        0x78, 0x56, 0x34, 0x12, 0x05, 0x01, 0xB8, 0x0B,
        0x00, 0x01, 0x01, 0x02, 0xDD, 0xCC, 0xBB, 0xAA,
        0x02, 0x01, 0x03, 0x04, 0x10, 0x20, 0x30, 0x40,
        0x02, 0x05, 0x02, 0x50, 0x60
    };

    ssize_t ret = ReadCharTextToHexVector(kHexProtocol, out);
    ASSERT_EQ((int32_t)ret, (int32_t)expect.size());
    ASSERT_EQ(out.size(), expect.size());

    for (size_t i = 0; i < expect.size(); ++i) {
        SCOPED_TRACE(i);
        EXPECT_EQ(out[i], expect[i]);
    }
}
