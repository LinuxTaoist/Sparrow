/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : TestLogFileName.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : LogFileName internal tests.
 *  @date       : 2026/09/20
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <cstdio>
#include <fstream>
#include <string>
#include <gtest/gtest.h>
#include "LogFileName.h"

namespace {

bool StartsWith(const std::string& value, const std::string& prefix) {
    return value.compare(0, prefix.size(), prefix) == 0;
}

bool EndsWith(const std::string& value, const std::string& suffix) {
    return value.size() >= suffix.size()
        && value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// 测试未指定格式时使用默认的基础文件名和扩展名
TEST(LogM_LogFileName, EmptyFormatUsesDefaultActiveFileFormat) {
    LogFileName name("main.log", "");
    std::string fileName;

    ASSERT_EQ(name.Build(fileName), 0);
    EXPECT_EQ(fileName, "main.log");
}

// 测试自定义格式能够替换基础名、启动标识、单调时间和扩展名令牌
TEST(LogM_LogFileName, CustomFormatReplacesAllTokens) {
    LogFileName name("main.log", "BN_BS-SI_MH_ST.FX");
    std::string fileName;

    ASSERT_EQ(name.Build(fileName), 0);
    EXPECT_TRUE(StartsWith(fileName, "main_"));
    EXPECT_NE(fileName.find("-"), std::string::npos);
    EXPECT_TRUE(EndsWith(fileName, ".log"));
}

// 测试省略可选令牌时仍能生成合法的动态文件名
TEST(LogM_LogFileName, OptionalTokensCanBeOmitted) {
    LogFileName name("main.log", "BN_ST.FX");
    std::string fileName;

    ASSERT_EQ(name.Build(fileName), 0);
    EXPECT_TRUE(StartsWith(fileName, "main_"));
    EXPECT_TRUE(EndsWith(fileName, ".log"));
    EXPECT_EQ(fileName.find("_00"), std::string::npos);
}

// 测试无扩展名文件不会产生多余的结尾点号
TEST(LogM_LogFileName, FileWithoutExtensionDoesNotLeaveTrailingDot) {
    LogFileName name("main", "BN.FX");
    std::string fileName;

    ASSERT_EQ(name.Build(fileName), 0);
    EXPECT_EQ(fileName, "main");
}

// 测试格式中的字面字符能够原样保留
TEST(LogM_LogFileName, LiteralCharactersArePreserved) {
    LogFileName name("main.log", "BN_[tag].FX");
    std::string fileName;

    ASSERT_EQ(name.Build(fileName), 0);
    EXPECT_EQ(fileName, "main_[tag].log");
}

// 测试空文件名输入返回失败
TEST(LogM_LogFileName, EmptyFileNameFails) {
    LogFileName name("", "BN.FX");
    std::string fileName;

    EXPECT_EQ(name.Build(fileName), -1);
}

} // namespace
