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
#include "GeneralUtils.h"

using ::testing::Test;
using namespace std;
using namespace GeneralUtils;

// 示例测试用例
TEST(Util_AbsValue, ForOne) {
    for (int32_t i = -100; i <= 100; i++) {
        EXPECT_EQ(AbsValue(i), i > 0 ? i : -i);
    }

    // for (double d = -100.0; d <= 100.0; d += 0.1) {
    //     EXPECT_DOUBLE_EQ(AbsValue(d), d > 0.0 ? d : -d);
    // }
}

TEST(Util_AbsValue, ForTwo) {
    for (int32_t i = -100; i <= 100; i++) {
        EXPECT_EQ(AbsValue(i, 0), i > 0 ? i : -i);
    }

    // for (double d = -100.0; d <= 100.0; d += 0.1) {
    //     EXPECT_DOUBLE_EQ(AbsValue(d, 0.0), d > 0.0 ? d : -d);
    // }
}

TEST(Util_GetRandomInteger, NormalCase) {
    for (int32_t i = 1; i < 5; i++) {
        int32_t random = GetRandomInteger(i);
        int32_t actual = (int32_t)std::to_string(random).length();
        EXPECT_EQ(i, actual);
    }
}

TEST(Util_GetRandomString, NormalCase) {
    for (int32_t i = 1; i < 5; i++) {
        EXPECT_EQ(i, (int32_t)GetRandomString(i).length());
    }
}

// 测试用例：正常情况，能找到第 n 个目标字符前的字符
TEST(Util_GetCharBeforeNthTarget, NormalCase) {
    std::string str = "abcdefg";
    char targetChar = 'd';
    int index = 1;
    char out;

    int result = GetCharBeforeNthTarget(str, targetChar, index, out);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(out, 'c');
}

// 测试用例：目标字符是字符串的第一个字符，找不到前一个字符
TEST(Util_GetCharBeforeNthTarget, TargetIsFirstCharCase) {
    std::string str = "abcdefg";
    char targetChar = 'a';
    int index = 1;
    char out;

    int result = GetCharBeforeNthTarget(str, targetChar, index, out);
    EXPECT_EQ(result, -1);
}

// 测试用例：字符串中不存在目标字符
TEST(Util_GetCharBeforeNthTarget, TargetCharNotFoundCase) {
    std::string str = "abcdefg";
    char targetChar = 'x';
    int index = 1;
    char out;

    int result = GetCharBeforeNthTarget(str, targetChar, index, out);
    EXPECT_EQ(result, -1);
}

// 测试用例：要求的目标字符索引超出实际出现次数
TEST(Util_GetCharBeforeNthTarget, IndexOutOfRangeCase) {
    std::string str = "abcda";
    char targetChar = 'a';
    int index = 3;
    char out;

    int result = GetCharBeforeNthTarget(str, targetChar, index, out);
    EXPECT_EQ(result, -1);
}

// 测试用例：空字符串
TEST(Util_GetCharBeforeNthTarget, EmptyStringCase) {
    std::string str = "";
    char targetChar = 'a';
    int index = 1;
    char out;

    int result = GetCharBeforeNthTarget(str, targetChar, index, out);
    EXPECT_EQ(result, -1);
}

TEST(Util_CountWords, NormalCase) {
    EXPECT_EQ(CountWords("hello"), 1);
    EXPECT_EQ(CountWords("hello world"), 2);
    EXPECT_EQ(CountWords("I am Lihua"), 3);
    EXPECT_EQ(CountWords("I am a student"), 4);
}

// 测试用例：正常情况，目标内存块在源内存块中存在
TEST(Util_FindSubMemory, NormalCase) {
    char src[] = "abcdefg";
    char tar[] = "cde";
    int sLen = sizeof(src) - 1;
    int tLen = sizeof(tar) - 1;

    void* result = FindSubMemory(src, sLen, tar, tLen);
    EXPECT_EQ(memcmp(result, tar, tLen), 0);
}

// 测试用例：目标内存块在源内存块中不存在
TEST(Util_FindSubMemory, NotFoundCase) {
    char src[] = "abcdefg";
    char tar[] = "xyz";
    int sLen = sizeof(src) - 1;
    int tLen = sizeof(tar) - 1;

    void* result = FindSubMemory(src, sLen, tar, tLen);
    EXPECT_TRUE(result == nullptr);
}

// 测试用例：源内存块为空指针
TEST(Util_FindSubMemory, NullSourceMemory) {
    char tar[] = "xyz";
    int sLen = 0;
    int tLen = sizeof(tar) - 1;

    void* result = FindSubMemory(nullptr, sLen, tar, tLen);
    EXPECT_TRUE(result == nullptr);
}

// 测试用例：目标内存块为空指针
TEST(Util_FindSubMemory, NullTargetMemory) {
    char src[] = "abcdefg";
    int sLen = sizeof(src) - 1;
    int tLen = 0;

    void* result = FindSubMemory(src, sLen, nullptr, tLen);
    EXPECT_TRUE(result == nullptr);
}

// 测试用例：源内存块长度小于目标内存块长度
TEST(Util_FindSubMemory, SourceShorterThanTarget) {
    char src[] = "ab";
    char tar[] = "abc";
    int sLen = sizeof(src) - 1;
    int tLen = sizeof(tar) - 1;

    void* result = FindSubMemory(src, sLen, tar, tLen);
    EXPECT_TRUE(result == nullptr);
}

// 测试用例：源内存块长度等于目标内存块长度，内容相同
TEST(Util_FindSubMemory, SameLengthAndSameContent) {
    char src[] = "abc";
    char tar[] = "abc";
    int sLen = sizeof(src) - 1;
    int tLen = sizeof(tar) - 1;

    void* result = FindSubMemory(src, sLen, tar, tLen);
    EXPECT_TRUE(result != nullptr);
    EXPECT_EQ(memcmp(result, tar, tLen), 0);
}
