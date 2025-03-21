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

// --------------------------------------------------------------------------------------------------------------------
// - Util_ToInteger
// --------------------------------------------------------------------------------------------------------------------
// 测试用例：正常情况，输入字符串长度足够
TEST(Util_ToInteger, NormalCase) {
    std::string input = "\x01\x02\x03\x04";
    uint32_t output;
    int32_t result = ToInteger(input, output);
    EXPECT_EQ(result, (int32_t)sizeof(uint32_t));
    uint32_t expected = 0x01020304;
    EXPECT_EQ(expected, output);
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
    uint16_t expected = 0x0506;
    EXPECT_EQ(expected, output);
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_ToString
// --------------------------------------------------------------------------------------------------------------------
// 测试用例：正常情况，针对 uint32_t 类型
TEST(Util_ToString, NormalCaseUint32) {
    uint32_t input = 0x01020304;
    std::string output;
    int32_t result = ToString(input, output);
    EXPECT_EQ(result, (int32_t)sizeof(uint32_t));
    std::string expected = "\x01\x02\x03\x04";
    EXPECT_EQ(expected, output);
}

// 测试用例：正常情况，针对 uint16_t 类型
TEST(Util_ToString, NormalCaseUint16) {
    uint16_t input = 0x0506;
    std::string output;
    int32_t result = ToString(input, output);
    EXPECT_EQ(result, (int32_t)sizeof(uint16_t));
    std::string expected = "\x05\x06";
    EXPECT_EQ(expected, output);
}

// 测试用例：测试输入为 0 的情况
TEST(Util_ToString, InputIsZero) {
    uint32_t input = 0;
    std::string output;
    std::string expected(4, '\x00');
    int32_t result = ToString(input, output);

    EXPECT_EQ(result, (int32_t)sizeof(uint32_t));
    EXPECT_EQ(output.size(), expected.size());
    if (output.size() == expected.size()) {
        for (size_t i = 0; i < output.size(); ++i) {
            EXPECT_EQ(output[i], expected[i]);
        }
    }
}

// 测试用例：测试最大可能值，针对 uint8_t 类型
TEST(Util_ToString, MaxValueUint8) {
    uint8_t input = 0xFF;
    std::string output;
    std::string expected = "\xFF";
    int32_t result = ToString(input, output);

    EXPECT_EQ(result, (int32_t)sizeof(uint8_t));
    EXPECT_EQ(expected, output);

}

// 测试用例：测试最小可能值，针对 uint8_t 类型
TEST(Util_ToString, MinValueUint8) {
    uint8_t input = 0x00;
    std::string output;
    std::string expected(1, '\x00');
    int32_t result = ToString(input, output);

    EXPECT_EQ(result, (int32_t)sizeof(uint8_t));
    EXPECT_EQ(expected.size(), output.size());
    if (expected.size() == output.size()) {
        for (size_t i = 0; i < expected.size(); ++i) {
            EXPECT_EQ(expected[i], output[i]);
        }
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_ToHexString
// --------------------------------------------------------------------------------------------------------------------
// 测试用例：正常情况，针对 uint8_t 类型的向量
TEST(Util_ToHexString, NormalCaseUint8) {
    std::vector<uint8_t> input = {0x01, 0x0F, 0xFF};
    std::string output = ToHexString(input);
    std::string expected = "010fff";
    EXPECT_EQ(expected, output);
}

// 测试用例：正常情况，针对 uint16_t 类型的向量
TEST(Util_ToHexString, NormalCaseUint16) {
    std::vector<uint16_t> input = {0x0102, 0x0304};
    std::string output = ToHexString(input);
    std::string expected = "01020304";
    EXPECT_EQ(expected, output);
}

// 测试用例：空向量情况
TEST(Util_ToHexString, EmptyVectorCase) {
    std::vector<uint8_t> input;
    std::string output = ToHexString(input);
    std::string expected = "";
    EXPECT_EQ(expected, output);
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_ToStringVector
// --------------------------------------------------------------------------------------------------------------------
// 测试用例：正常情况，针对 uint8_t 类型的向量
TEST(Util_ToStringVector, NormalCaseUint8) {
    std::vector<uint8_t> input = {0x01, 0x02, 0x03};
    std::string output = ToString(input);
    std::string expected = "\x01\x02\x03";
    EXPECT_EQ(expected, output);
}

// 测试用例：正常情况，针对 uint16_t 类型的向量
TEST(Util_ToStringVector, NormalCaseUint16) {
    std::vector<uint16_t> input = {0x0102, 0x0304};
    std::string output = ToString(input);
    std::string expected = "\x01\x02\x03\x04";
    EXPECT_EQ(expected, output);
}

// 测试用例：空向量情况
TEST(Util_ToStringVector, EmptyVectorCase) {
    std::vector<uint8_t> input;
    std::string output = ToString(input);
    std::string expected = "";
    EXPECT_EQ(expected, output);
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_HexStringToAscii
// --------------------------------------------------------------------------------------------------------------------
// 测试用例：正常情况，针对字符串 "Hello"
TEST(Util_HexStringToAscii, NormalHexString) {
    std::string hexString = "48656C6C6F";
    std::string result = HexStringToAscii(hexString);
    std::string expected = "Hello";
    EXPECT_EQ(expected, result);
}

// 测试用例：正常情况，针对字符串 "Hello"（小写字母）
TEST(Util_HexStringToAscii, LowercaseHexString) {
    std::string hexString = "48656c6c6f";
    std::string result = HexStringToAscii(hexString);
    std::string expected = "Hello";
    EXPECT_EQ(expected, result);
}

// 测试用例：正常情况，针对字符串 "0123456789"
TEST(Util_HexStringToAscii, NumbersToAscii) {
    std::string hexString = "30313233343536373839";
    std::string result = HexStringToAscii(hexString);
    std::string expected = "0123456789";
    EXPECT_EQ(expected, result);
}

// 测试用例：空字符串情况
TEST(Util_HexStringToAscii, EmptyString) {
    std::string hexString = "";
    std::string result = HexStringToAscii(hexString);
    std::string expected = "";
    EXPECT_EQ(expected, result);
}

// 测试用例：奇数长度字符串情况
TEST(Util_HexStringToAscii, OddLengthString) {
    std::string hexString = "123";
    std::string result = HexStringToAscii(hexString);
    std::string expected = "";
    EXPECT_EQ(expected, result);
}

// 测试用例：非十六进制字符情况
TEST(Util_HexStringToAscii, NonHexCharacters) {
    std::string hexString = "1G23";
    std::string result = HexStringToAscii(hexString);
    std::string expected = "";
    EXPECT_EQ(expected, result);
}
