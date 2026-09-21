#include <cstdio>
#include <fstream>
#include <string>
#include <gtest/gtest.h>
#include "LogFileName.h"

namespace {

bool StartsWith(const std::string& value, const std::string& prefix)
{
    return value.compare(0, prefix.size(), prefix) == 0;
}

bool EndsWith(const std::string& value, const std::string& suffix)
{
    return value.size() >= suffix.size()
        && value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

TEST(LogM_LogFileName, EmptyFormatUsesDefaultActiveFileFormat)
{
    LogFileName name("main.log", "");
    std::string fileName;

    ASSERT_EQ(name.Build(fileName), 0);
    EXPECT_EQ(fileName, "main.log");
}

TEST(LogM_LogFileName, CustomFormatReplacesAllTokens)
{
    LogFileName name("main.log", "BN_BS-SI_MH_ST.FX");
    std::string fileName;

    ASSERT_EQ(name.Build(fileName), 0);
    EXPECT_TRUE(StartsWith(fileName, "main_"));
    EXPECT_NE(fileName.find("-"), std::string::npos);
    EXPECT_TRUE(EndsWith(fileName, ".log"));
}

TEST(LogM_LogFileName, OptionalTokensCanBeOmitted)
{
    LogFileName name("main.log", "BN_ST.FX");
    std::string fileName;

    ASSERT_EQ(name.Build(fileName), 0);
    EXPECT_TRUE(StartsWith(fileName, "main_"));
    EXPECT_TRUE(EndsWith(fileName, ".log"));
    EXPECT_EQ(fileName.find("_00"), std::string::npos);
}

TEST(LogM_LogFileName, FileWithoutExtensionDoesNotLeaveTrailingDot)
{
    LogFileName name("main", "BN.FX");
    std::string fileName;

    ASSERT_EQ(name.Build(fileName), 0);
    EXPECT_EQ(fileName, "main");
}

TEST(LogM_LogFileName, LiteralCharactersArePreserved)
{
    LogFileName name("main.log", "BN_[tag].FX");
    std::string fileName;

    ASSERT_EQ(name.Build(fileName), 0);
    EXPECT_EQ(fileName, "main_[tag].log");
}

TEST(LogM_LogFileName, EmptyFileNameFails)
{
    LogFileName name("", "BN.FX");
    std::string fileName;

    EXPECT_EQ(name.Build(fileName), -1);
}

} // namespace
