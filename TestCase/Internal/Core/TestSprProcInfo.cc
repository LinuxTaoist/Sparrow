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
#include "SprProcInfo.h"
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
