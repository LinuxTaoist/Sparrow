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

// --------------------------------------------------------------------------------------------------------------------
// - Util_AbsValue
// --------------------------------------------------------------------------------------------------------------------
// 测试用例：一个自然数绝对值
TEST(Util_AbsValue, ForOne) {
    for (int32_t i = -100; i <= 100; i++) {
        EXPECT_EQ(AbsValue(i), i > 0 ? i : -i);
    }

    // for (double d = -100.0; d <= 100.0; d += 0.1) {
    //     EXPECT_DOUBLE_EQ(AbsValue(d), d > 0.0 ? d : -d);
    // }
}

// 测试用例：差值绝对值
TEST(Util_AbsValue, ForTwo) {
    for (int32_t i = -100; i <= 100; i++) {
        EXPECT_EQ(AbsValue(i, 0), i > 0 ? i : -i);
    }

    // for (double d = -100.0; d <= 100.0; d += 0.1) {
    //     EXPECT_DOUBLE_EQ(AbsValue(d, 0.0), d > 0.0 ? d : -d);
    // }
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_GetRandomInteger
// --------------------------------------------------------------------------------------------------------------------
// 测试用例：正常情况
TEST(Util_GetRandomInteger, NormalCase) {
    for (int32_t i = 1; i < 5; i++) {
        int32_t random = GetRandomInteger(i);
        int32_t actual = (int32_t)std::to_string(random).length();
        EXPECT_EQ(i, actual);
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_GetRandomString
// --------------------------------------------------------------------------------------------------------------------
// 测试用例：正常情况
TEST(Util_GetRandomString, NormalCase) {
    for (int32_t i = 1; i < 5; i++) {
        EXPECT_EQ(i, (int32_t)GetRandomString(i).length());
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_GetCharBeforeNthTarget
// --------------------------------------------------------------------------------------------------------------------
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

// --------------------------------------------------------------------------------------------------------------------
// - Util_CountWords
// --------------------------------------------------------------------------------------------------------------------
TEST(Util_CountWords, NormalCase) {
    EXPECT_EQ(CountWords("hello"), 1);
    EXPECT_EQ(CountWords("hello world"), 2);
    EXPECT_EQ(CountWords("I am Lihua"), 3);
    EXPECT_EQ(CountWords("I am a student"), 4);
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_FindSubMemory
// --------------------------------------------------------------------------------------------------------------------
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

// --------------------------------------------------------------------------------------------------------------------
// - Util_Split
// --------------------------------------------------------------------------------------------------------------------
// 测试按分隔符拆分字符串
TEST(Util_Split, BasicSplit) {
    std::vector<std::string> tokens = Split("a,b,c", ',');
    ASSERT_EQ(tokens.size(), 3u);
    EXPECT_EQ(tokens[0], "a");
    EXPECT_EQ(tokens[1], "b");
    EXPECT_EQ(tokens[2], "c");
}

// 测试无分隔符时拆分结果
TEST(Util_Split, NoDelimiter) {
    std::vector<std::string> tokens = Split("hello", ',');
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0], "hello");
}

// 测试空字符串拆分结果
TEST(Util_Split, EmptyString) {
    std::vector<std::string> tokens = Split("", ',');
    EXPECT_TRUE(tokens.empty());
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_GetSubstringAfterLastDelimiter
// --------------------------------------------------------------------------------------------------------------------
// 测试获取最后一个分隔符后的子串
TEST(Util_GetSubstringAfterLastDelimiter, NormalCase) {
    EXPECT_EQ(GetSubstringAfterLastDelimiter("/usr/local/bin/app", '/'), "app");
    EXPECT_EQ(GetSubstringAfterLastDelimiter("a.b.c", '.'), "c");
}

// 测试无分隔符或末尾分隔符的边界情况
TEST(Util_GetSubstringAfterLastDelimiter, NoDelimiterOrTrailing) {
    EXPECT_EQ(GetSubstringAfterLastDelimiter("no-delimiter", '/'), "");
    EXPECT_EQ(GetSubstringAfterLastDelimiter("trailing/", '/'), "");
    EXPECT_EQ(GetSubstringAfterLastDelimiter("", '/'), "");
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_GetCharAfterNthTarget
// --------------------------------------------------------------------------------------------------------------------
// 测试获取第 n 个目标字符后的字符
TEST(Util_GetCharAfterNthTarget, NormalCase) {
    char out = 0;
    EXPECT_EQ(GetCharAfterNthTarget("a1b2c3", 'b', 1, out), 0);
    EXPECT_EQ(out, '2');
}

// 测试目标字符在末尾或不存在等边界情况
TEST(Util_GetCharAfterNthTarget, BoundaryCases) {
    char out = 0;
    // 目标是最后一个字符，找不到后一个字符
    EXPECT_EQ(GetCharAfterNthTarget("abc", 'c', 1, out), -1);
    // 不存在
    EXPECT_EQ(GetCharAfterNthTarget("abc", 'x', 1, out), -1);
    // 空字符串
    EXPECT_EQ(GetCharAfterNthTarget("", 'a', 1, out), -1);
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_GetCurTimeStr
// --------------------------------------------------------------------------------------------------------------------
// 测试当前时间字符串格式
TEST(Util_GetCurTimeStr, FormatCheck) {
    std::string timeStr = GetCurTimeStr();
    // 格式: YYYY-MM-DD HH:MM:SS.mmm
    ASSERT_EQ(timeStr.size(), 23u);
    EXPECT_EQ(timeStr[4], '-');
    EXPECT_EQ(timeStr[7], '-');
    EXPECT_EQ(timeStr[10], ' ');
    EXPECT_EQ(timeStr[13], ':');
    EXPECT_EQ(timeStr[16], ':');
    EXPECT_EQ(timeStr[19], '.');
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_SystemCmd
// --------------------------------------------------------------------------------------------------------------------
// 测试无输出的系统命令执行
TEST(Util_SystemCmd, EchoWithoutOutput) {
    EXPECT_EQ(SystemCmd("echo hello > /dev/null"), 0);
}

// 测试带输出的系统命令执行
TEST(Util_SystemCmd, EchoWithOutput) {
    std::string out;
    EXPECT_EQ(SystemCmd(out, "echo hello"), 0);
    EXPECT_EQ(out, "hello\n");
}

// 测试失败的系统命令返回错误
TEST(Util_SystemCmd, FailingCommand) {
    std::string out;
    EXPECT_EQ(SystemCmd(out, "exit 1"), -1);
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_InitSignalHandler
// --------------------------------------------------------------------------------------------------------------------
// 测试注册信号处理函数
TEST(Util_InitSignalHandler, RegisterHandler) {
    auto handler = [](int) {};
    EXPECT_EQ(InitSignalHandler(handler), 0);
}

// --------------------------------------------------------------------------------------------------------------------
// - Util_GetRandomInteger 边界
// --------------------------------------------------------------------------------------------------------------------
// 测试随机整数的非法宽度边界
TEST(Util_GetRandomInteger, BoundaryWidth) {
    EXPECT_EQ(GetRandomInteger(0), 0);
    EXPECT_EQ(GetRandomInteger(-5), 0);
}
