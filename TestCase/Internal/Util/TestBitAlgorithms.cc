/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestBitAlgorithms.cc
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

// 测试用例：正常情况，针对 uint32_t 类型
TEST(Util_GetValueInRange, Uint32_NormalCase) {
    uint32_t value = 0x12345678;
    int startBit = 8;
    int endBit = 15;
    uint32_t expected = 0x56;
    uint32_t result;
    bool success = GetValueInRange(value, startBit, endBit, result);
    EXPECT_TRUE(success);
    EXPECT_EQ(expected, result);
}

// 测试用例：startBit 等于 endBit，针对 uint16_t 类型
TEST(Util_GetValueInRange, Uint16_StartBitEqualsEndBitCase) {
    uint16_t value = 0xABCD;
    int startBit = 4;
    int endBit = 4;
    uint16_t expected = 0;
    uint16_t result;
    bool success = GetValueInRange(value, startBit, endBit, result);
    EXPECT_TRUE(success);
    EXPECT_EQ(expected, result);
}

// 测试用例：startBit 为 0，针对 uint8_t 类型
TEST(Util_GetValueInRange, Uint8_StartBitZeroCase) {
    uint8_t value = 0xAB;
    int startBit = 0;
    int endBit = 3;
    uint8_t expected = 0xB;
    uint8_t result;
    bool success = GetValueInRange(value, startBit, endBit, result);
    EXPECT_TRUE(success);
    EXPECT_EQ(expected, result);
}

// 测试用例：endBit 为类型最大位，针对 uint32_t 类型
TEST(Util_GetValueInRange, Uint32_EndBitMaxCase) {
    uint32_t value = 0xFFFFFFFF;
    int startBit = 24;
    int endBit = 31;
    uint32_t expected = 0xFF;
    uint32_t result;
    bool success = GetValueInRange(value, startBit, endBit, result);
    EXPECT_TRUE(success);
    EXPECT_EQ(expected, result);
}

// 测试用例：startBit 超出类型范围，针对 uint64_t 类型
TEST(Util_GetValueInRange, Uint64_StartBitOutOfRangeCase) {
    uint64_t value = 0x123456789ABCDEF0;
    int startBit = 64;
    int endBit = 65;
    uint64_t result;
    bool success = GetValueInRange(value, startBit, endBit, result);
    EXPECT_FALSE(success);
}

// 测试用例：endBit 超出类型范围，针对 uint64_t 类型
TEST(Util_GetValueInRange, Uint64_EndBitOutOfRangeCase) {
    uint64_t value = 0x123456789ABCDEF0;
    int startBit = 0;
    int endBit = 64;
    uint64_t result;
    bool success = GetValueInRange(value, startBit, endBit, result);
    EXPECT_FALSE(success);
}

// 测试用例：startBit 大于 endBit，针对 uint16_t 类型
TEST(Util_GetValueInRange, Uint16_StartBitGreaterThanEndBitCase) {
    uint16_t value = 0xABCD;
    int startBit = 10;
    int endBit = 5;
    uint16_t result;
    bool success = GetValueInRange(value, startBit, endBit, result);
    EXPECT_FALSE(success);
}

// 测试用例：startBit 和 endBit 均为负数，针对 uint8_t 类型
TEST(Util_GetValueInRange, Uint8_NegativeStartAndEndBitCase) {
    uint8_t value = 0xAB;
    int startBit = -1;
    int endBit = -2;
    uint8_t result;
    bool success = GetValueInRange(value, startBit, endBit, result);
    EXPECT_FALSE(success);
}
