/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : TestLogConfiger.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : LogConfiger internal tests.
 *  @date       : 2026/09/20
 *
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <cstdio>
#include <fstream>
#include <string>

#include "LogConfiger.h"
#include "gtest/gtest.h"

namespace {

const char* TEST_CONFIG_PATH = "/tmp/sparrow_log_configer_test.ini";

class LogM_LogConfiger : public ::testing::Test
{
protected:
    void TearDown() override
    {
        std::remove(TEST_CONFIG_PATH);
    }

    void WriteConfig(const std::string& content)
    {
        std::ofstream file(TEST_CONFIG_PATH);
        ASSERT_TRUE(file.is_open());
        file << content;
    }
};

} // namespace

// 测试 default 和模块配置整合为完整模块配置
TEST_F(LogM_LogConfiger, LoadAndMergeModuleConfiguration)
{
    WriteConfig(
        "[output.default]\n"
        "frame_length_bytes=2048\n"
        "enabled=true\n"
        "level=info\n"
        "output=file\n"
        "file_path=/tmp/default\n"
        "file_name=default.log\n"
        "file_name_format=BN_BS-SI_MH_ST.FX\n"
        "[output.NetworkSrv]\n"
        "file_path=/tmp/network\n"
        "flush_count=32\n"
        "[output.DebugSrv]\n"
        "output=stdout\n");

    LogConfiger configer;
    ASSERT_EQ(configer.Load(TEST_CONFIG_PATH), 0);
    LogConfiger::LogModules config;
    ASSERT_EQ(configer.GetLogModules(config), 0);

    ASSERT_EQ(config.size(), 3U);
    ASSERT_EQ(config.at("default").at("frame_length_bytes"), "2048");
    EXPECT_EQ(config.at("default").at("level"), "info");
    EXPECT_EQ(config.at("default").at("file_name"), "default.log");
    EXPECT_EQ(config.at("default").at("file_name_format"),
              "BN_BS-SI_MH_ST.FX");
    EXPECT_EQ(config.at("NetworkSrv").at("file_name"), "default.log");
    EXPECT_EQ(config.at("NetworkSrv").at("file_name_format"),
              "BN_BS-SI_MH_ST.FX");
    EXPECT_EQ(config.at("NetworkSrv").at("file_path"), "/tmp/network");
    EXPECT_EQ(config.at("NetworkSrv").at("flush_count"), "32");
    EXPECT_EQ(config.at("DebugSrv").at("file_name"), "default.log");
    EXPECT_EQ(config.at("DebugSrv").at("output"), "stdout");
}

// 测试模块缺少配置项时使用 default 生成完整配置
TEST_F(LogM_LogConfiger, MissingModuleKeysUseMergedDefaults)
{
    WriteConfig(
        "[output.default]\n"
        "file_name=default.log\n"
        "[output.NetworkSrv]\n"
        "file_num=2\n");

    LogConfiger configer;
    ASSERT_EQ(configer.Load(TEST_CONFIG_PATH), 0);
    LogConfiger::LogModules allModules;
    ASSERT_EQ(configer.GetLogModules(allModules), 0);
    const LogConfiger::LogModuleAttrs& config = allModules.at("NetworkSrv");

    EXPECT_EQ(config.at("file_name"), "default.log");
    EXPECT_EQ(config.at("file_name_format"), "");
    EXPECT_EQ(config.at("file_num"), "2");
    EXPECT_EQ(config.at("level"), "debug");
    EXPECT_EQ(config.at("flush_interval_ms"), "1000");
}

// 测试没有配置文件时仍保留完整的 default 模块配置
TEST_F(LogM_LogConfiger, MissingFileKeepsDefaultConfiguration)
{
    LogConfiger configer;
    EXPECT_EQ(configer.Load("/tmp/sparrow_log_configer_missing.ini"), -1);

    LogConfiger::LogModules config;
    ASSERT_EQ(configer.GetLogModules(config), 0);
    ASSERT_EQ(config.size(), 1U);
    EXPECT_EQ(config.at("default").at("file_name"), "sprlog.log");
    EXPECT_EQ(config.at("default").at("flush_interval_ms"), "1000");
}
