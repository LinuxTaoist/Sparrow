/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestGeneralUtils.cc
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
#include <string.h>
#include "gtest/gtest.h"
#include "BitAlgorithms.h"

using namespace BitAlgorithms;

// 测试用例：正常情况，获取部分位的值，针对 uint8_t 类型
TEST(Util_GetValueInRange_Uint8, NormalCase) {
    uint8_t value = 0xAB;
    int startBit = 4;
    int endBit = 7;
    uint8_t result = GetValueInRange(value, startBit, endBit);
    uint8_t expected = (value >> startBit) & ((1u << (endBit - startBit + 1)) - 1);
    EXPECT_EQ(result, expected);
}

// 测试用例：startBit 和 endBit 相同，获取单个位的值，针对 uint8_t 类型
TEST(Util_GetValueInRange_Uint8, SingleBitCase) {
    uint8_t value = 0xAB;
    int startBit = 6;
    int endBit = 6;
    uint8_t result = GetValueInRange(value, startBit, endBit);
    uint8_t expected = (value & (1u << startBit)) >> startBit;
    EXPECT_EQ(result, expected);
}

// 测试用例：startBit 为 0，endBit 为最大值，获取整个值，针对 uint8_t 类型
TEST(Util_GetValueInRange_Uint8, FullRangeCase) {
    uint8_t value = 0xAB;
    int startBit = 0;
    int endBit = sizeof(uint8_t) * 8 - 1;
    uint8_t result = GetValueInRange(value, startBit, endBit);
    EXPECT_EQ(result, value);
}

// 测试用例：endBit 超出类型范围，针对 uint8_t 类型
TEST(Util_GetValueInRange_Uint8, EndBitOutOfRangeCase) {
    uint8_t value = 0xAB;
    int startBit = 0;
    int endBit = 63;
    uint8_t result = GetValueInRange(value, startBit, endBit);
    EXPECT_EQ(result, value);
}

// 测试用例：正常情况，获取部分位的值，针对 uint16_t 类型
TEST(Util_GetValueInRange_Uint16, NormalCase) {
    uint16_t value = 0x1234;
    int startBit = 4;
    int endBit = 11;
    uint16_t result = GetValueInRange(value, startBit, endBit);
    uint16_t expected = (value >> startBit) & ((1u << (endBit - startBit + 1)) - 1);
    EXPECT_EQ(result, expected);
}

// 测试用例：startBit 和 endBit 相同，获取单个位的值，针对 uint16_t 类型
TEST(Util_GetValueInRange_Uint16, SingleBitCase) {
    uint16_t value = 0x1234;
    int startBit = 10;
    int endBit = 10;
    uint16_t result = GetValueInRange(value, startBit, endBit);
    uint16_t expected = (value & (1u << startBit)) >> startBit;
    EXPECT_EQ(result, expected);
}

// 测试用例：startBit 为 0，endBit 为最大值，获取整个值，针对 uint16_t 类型
TEST(Util_GetValueInRange_Uint16, FullRangeCase) {
    uint16_t value = 0x1234;
    int startBit = 0;
    int endBit = sizeof(uint16_t) * 8 - 1;
    uint16_t result = GetValueInRange(value, startBit, endBit);
    EXPECT_EQ(result, value);
}

// 测试用例：endBit 超出类型范围，针对 uint16_t 类型
TEST(Util_GetValueInRange_Uint16, EndBitOutOfRangeCase) {
    uint16_t value = 0x1234;
    int startBit = 0;
    int endBit = 63;
    uint16_t result = GetValueInRange(value, startBit, endBit);
    EXPECT_EQ(result, value);
}

// 测试用例：正常情况，获取部分位的值，针对 uint32_t 类型
TEST(Util_GetValueInRange_Uint32, NormalCase) {
    uint32_t value = 0x12345678;
    int startBit = 8;
    int endBit = 19;
    uint32_t result = GetValueInRange(value, startBit, endBit);
    uint32_t expected = (value >> startBit) & ((1u << (endBit - startBit + 1)) - 1);
    EXPECT_EQ(result, expected);
}

// 测试用例：startBit 和 endBit 相同，获取单个位的值，针对 uint32_t 类型
TEST(Util_GetValueInRange_Uint32, SingleBitCase) {
    uint32_t value = 0x12345678;
    int startBit = 10;
    int endBit = 10;
    uint32_t result = GetValueInRange(value, startBit, endBit);
    uint32_t expected = (value & (1u << startBit)) >> startBit;
    EXPECT_EQ(result, expected);
}

// 测试用例：startBit 为 0，endBit 为最大值，获取整个值，针对 uint32_t 类型
TEST(Util_GetValueInRange_Uint32, FullRangeCase) {
    uint32_t value = 0x12345678;
    int startBit = 0;
    int endBit = sizeof(uint32_t) * 8 - 1;
    uint32_t result = GetValueInRange(value, startBit, endBit);
    EXPECT_EQ(result, value);
}

// 测试用例：endBit 超出类型范围，针对 uint32_t 类型
TEST(Util_GetValueInRange_Uint32, EndBitOutOfRangeCase) {
    uint32_t value = 0x12345678;
    int startBit = 0;
    int endBit = 63;
    uint32_t result = GetValueInRange(value, startBit, endBit);
    EXPECT_EQ(result, value);
}

// 测试用例：正常情况，获取部分位的值，针对 uint64_t 类型
TEST(Util_GetValueInRange_Uint64, NormalCase) {
    uint64_t value = 0x123456789ABCDEF0;
    int startBit = 16;
    int endBit = 31;
    uint64_t result = GetValueInRange(value, startBit, endBit);
    uint64_t expected = (value >> startBit) & ((1ULL << (endBit - startBit + 1)) - 1);
    EXPECT_EQ(result, expected);
}

// 测试用例：startBit 和 endBit 相同，获取单个位的值，针对 uint64_t 类型
TEST(Util_GetValueInRange_Uint64, SingleBitCase) {
    uint64_t value = 0x123456789ABCDEF0;
    int startBit = 20;
    int endBit = 20;
    uint64_t result = GetValueInRange(value, startBit, endBit);
    uint64_t expected = (value & (1ULL << startBit)) >> startBit;
    EXPECT_EQ(result, expected);
}

// 测试用例：startBit 为 0，endBit 为最大值，获取整个值，针对 uint64_t 类型
TEST(Util_GetValueInRange_Uint64, FullRangeCase) {
    uint64_t value = 0x123456789ABCDEF0;
    int startBit = 0;
    int endBit = sizeof(uint64_t) * 8 - 1;
    uint64_t result = GetValueInRange(value, startBit, endBit);
    EXPECT_EQ(result, value);
}

// 测试用例：endBit 超出类型范围，针对 uint64_t 类型
TEST(Util_GetValueInRange_Uint64, EndBitOutOfRangeCase) {
    uint64_t value = 0x123456789ABCDEF0;
    int startBit = 0;
    int endBit = 63;
    uint64_t result = GetValueInRange(value, startBit, endBit);
    EXPECT_EQ(result, value);
}
