/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprProcInfo.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/04/19
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/04/19 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include "SprProcInfo.h"
#include "CommonMacros.h"
#include "gtest/gtest.h"

// 测试 SprProcInfo::GetDebugPath 方法
TEST(Core_SprProcInfo, GetDebugPathReturnsValidPath) {
    SprProcInfo* instance = SprProcInfo::GetInstance();

    std::string debugPath = instance->GetDebugPath();
    EXPECT_TRUE(debugPath.empty());

    instance->Init();
    debugPath = instance->GetDebugPath();
    EXPECT_FALSE(debugPath.empty());

    uint64_t expectTimeUs = 200000;
    usleep(expectTimeUs);

    uint64_t diffTimeUs = 20000;
    uint64_t runTimeUs = instance->GetRunTimeUs();
    EXPECT_GE(expectTimeUs, runTimeUs - diffTimeUs);
    EXPECT_LE(expectTimeUs, runTimeUs + diffTimeUs);
}

// 测试 SprProcInfo::GetBootTimeUs 方法
TEST(Core_SprProcInfo, GetBootTimeUs) {
    SprProcInfo* instance = SprProcInfo::GetInstance();
    uint64_t bootTimeUs = instance->GetBootTimeUs();
    EXPECT_TRUE(bootTimeUs > 0);
}

// 测试 SprProcInfo::GetBootTimeString 方法
TEST(Core_SprProcInfo, GetBootTimeString) {
    SprProcInfo* instance = SprProcInfo::GetInstance();
    std::string bootTimeString = instance->GetBootTimeString();
    EXPECT_FALSE(bootTimeString.empty());
}

// 测试 SprProcInfo::GetProcName 方法
TEST(Core_SprProcInfo, GetProcNameReturnsValidName) {
    SprProcInfo* instance = SprProcInfo::GetInstance();
    std::string procName = instance->GetProcName();
    EXPECT_FALSE(procName.empty());
}

// 涉及环境变量与临时目录的用例用 fixture 统一清理，避免影响其他测试
class Core_SprProcInfo_Env : public ::testing::Test {
protected:
    void TearDown() override {
        unsetenv(ENV_SPR_ROOT_PATH);
        remove("/tmp/sparrow_procinfo_root/Etc");
        remove("/tmp/sparrow_procinfo_root");
    }
};

// 测试设置环境变量时获取运行根路径
TEST_F(Core_SprProcInfo_Env, GetRunRootPathUsesEnvVariableWhenSet) {
    SprProcInfo* instance = SprProcInfo::GetInstance();
    const std::string expectRoot = "/tmp/sparrow_env_root_for_test";

    ASSERT_EQ(0, setenv(ENV_SPR_ROOT_PATH, expectRoot.c_str(), 1));
    EXPECT_EQ(expectRoot, instance->GetRunRootPath());
}

// 测试获取 Etc 路径时校验目录存在性
TEST_F(Core_SprProcInfo_Env, GetRunEtcPathChecksExistence) {
    SprProcInfo* instance = SprProcInfo::GetInstance();
    const std::string rootPath = "/tmp/sparrow_procinfo_root";
    const std::string etcPath = rootPath + "/Etc";

    remove(etcPath.c_str());
    remove(rootPath.c_str());
    ASSERT_EQ(0, mkdir(rootPath.c_str(), 0755));
    ASSERT_EQ(0, mkdir(etcPath.c_str(), 0755));
    ASSERT_EQ(0, setenv(ENV_SPR_ROOT_PATH, rootPath.c_str(), 1));

    EXPECT_EQ(etcPath, instance->GetRunEtcPath());

    ASSERT_EQ(0, remove(etcPath.c_str()));
    EXPECT_EQ("", instance->GetRunEtcPath());
}
