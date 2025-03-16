/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestGeneralConversions.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/03/16
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/03/16 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "gtest/gtest.h"
#include "GeneralConversions.h"

using namespace std;
using namespace GeneralConversions;

// 测试用例：正常情况，输入字符串长度足够
TEST(Util_ToInteger, NormalCase) {
    std::string input = "\x01\x02\x03\x04";
    uint32_t output;
    int32_t result = ToInteger(input, output);
    EXPECT_EQ(result, (int32_t)sizeof(uint32_t));
    uint32_t expected = (1u << 24) | (2u << 16) | (3u << 8) | 4u;
    EXPECT_EQ(output, expected);
}

// 测试用例：输入字符串长度不足
TEST(Util_ToInteger, StringLengthTooShortCase) {
    std::string input = "\x01";
    uint32_t output;
    int32_t result = ToInteger(input, output);
    EXPECT_EQ(result, -1);
}

// 测试用例：输入字符串长度等于类型大小
TEST(Util_ToInteger, StringLengthEqualsTypeSizeCase) {
    std::string input = "\x05\x06";
    uint16_t output;
    int32_t result = ToInteger(input, output);
    EXPECT_EQ(result, (int32_t)sizeof(uint16_t));
    uint16_t expected = (5u << 8) | 6u;
    EXPECT_EQ(output, expected);
}

// 测试用例：正常情况，针对 uint32_t 类型
TEST(Util_ToString, NormalCaseUint32) {
    uint32_t input = 0x01020304;
    std::string output;
    int32_t result = ToString(input, output);
    EXPECT_EQ(result, (int32_t)sizeof(uint32_t));
    std::string expected;
    for (int32_t i = 3; i >= 0; --i) {
        expected.push_back(static_cast<char>((input >> (i * 8)) & 0xFF));
    }
    EXPECT_EQ(output, expected);
}

// 测试用例：正常情况，针对 uint16_t 类型
TEST(Util_ToString, NormalCaseUint16) {
    uint16_t input = 0x0506;
    std::string output;
    int32_t result = ToString(input, output);
    EXPECT_EQ(result, (int32_t)sizeof(uint16_t));
    std::string expected;
    for (int32_t i = 1; i >= 0; --i) {
        expected.push_back(static_cast<char>((input >> (i * 8)) & 0xFF));
    }
    EXPECT_EQ(output, expected);
}

// 测试用例：测试输入为 0 的情况
TEST(Util_ToString, InputIsZero) {
    uint32_t input = 0;
    std::string output;
    int32_t result = ToString(input, output);
    EXPECT_EQ(result, (int32_t)sizeof(uint32_t));
    std::string expected(sizeof(uint32_t), '\0');
    EXPECT_EQ(output, expected);
}

// 测试用例：测试最大可能值，针对 uint8_t 类型
TEST(Util_ToString, MaxValueUint8) {
    uint8_t input = 0xFF;
    std::string output;
    int32_t result = ToString(input, output);
    EXPECT_EQ(result, (int32_t)sizeof(uint8_t));
    std::string expected;
    expected.push_back(static_cast<char>(input));
    EXPECT_EQ(output, expected);
}

// 测试用例：测试最小可能值，针对 uint8_t 类型
TEST(Util_ToString, MinValueUint8) {
    uint8_t input = 0x00;
    std::string output;
    int32_t result = ToString(input, output);
    EXPECT_EQ(result, (int32_t)sizeof(uint8_t));
    std::string expected;
    expected.push_back(static_cast<char>(input));
    EXPECT_EQ(output, expected);
}

// 测试用例：正常情况，针对 uint8_t 类型的向量
TEST(Util_ToHexString, NormalCaseUint8) {
    std::vector<uint8_t> input = {0x01, 0x0F, 0xFF};
    std::string output = ToHexString(input);
    std::string expected = "010fff";
    EXPECT_EQ(output, expected);
}

// 测试用例：正常情况，针对 uint16_t 类型的向量
TEST(Util_ToHexString, NormalCaseUint16) {
    std::vector<uint16_t> input = {0x0102, 0x0304};
    std::string output = ToHexString(input);
    std::string expected = "01020304";
    EXPECT_EQ(output, expected);
}

// 测试用例：空向量情况
TEST(Util_ToHexString, EmptyVectorCase) {
    std::vector<uint8_t> input;
    std::string output = ToHexString(input);
    std::string expected = "";
    EXPECT_EQ(output, expected);
}

// 测试用例：正常情况，针对 uint8_t 类型的向量
TEST(Util_ToStringVector, NormalCaseUint8) {
    std::vector<uint8_t> input = {0x01, 0x02, 0x03};
    std::string output = ToString(input);
    std::string expected;
    for (uint8_t val : input) {
        expected.push_back(static_cast<char>(val));
    }
    EXPECT_EQ(output, expected);
}

// 测试用例：正常情况，针对 uint16_t 类型的向量
TEST(Util_ToStringVector, NormalCaseUint16) {
    std::vector<uint16_t> input = {0x0102, 0x0304};
    std::string output = ToString(input);
    std::string expected;
    for (uint16_t val : input) {
        expected.push_back(static_cast<char>((val >> 8) & 0xFF));
        expected.push_back(static_cast<char>(val & 0xFF));
    }
    EXPECT_EQ(output, expected);
}

// 测试用例：空向量情况
TEST(Util_ToStringVector, EmptyVectorCase) {
    std::vector<uint8_t> input;
    std::string output = ToString(input);
    std::string expected = "";
    EXPECT_EQ(output, expected);
}
