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
