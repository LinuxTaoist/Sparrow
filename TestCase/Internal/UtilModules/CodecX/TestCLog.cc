/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestCLog.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : CLog 编解码框架日志封装内部测试
 *  @date       : 2026/09/10
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/10 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <atomic>
#include <string>
#include "CLog.h"
#include "gtest/gtest.h"

// 测试单例模式返回同一实例
TEST(UtilModules_CLog, SingletonReturnsSameInstance)
{
    CLog& a = CLog::GetInstance();
    CLog& b = CLog::GetInstance();
    EXPECT_EQ(&a, &b);
}

// 测试设置与获取日志等级
TEST(UtilModules_CLog, SetAndGetLevel)
{
    CLog& log = CLog::GetInstance();

    log.SetLevel(CLOG_LEVEL_ERROR);
    EXPECT_EQ(log.GetLevel(), CLOG_LEVEL_ERROR);

    log.SetLevel(CLOG_LEVEL_DEBUG);
    EXPECT_EQ(log.GetLevel(), CLOG_LEVEL_DEBUG);

    // 恢复默认等级，避免影响其他用例
    log.SetLevel(CLOG_LEVEL_INFO);
    EXPECT_EQ(log.GetLevel(), CLOG_LEVEL_INFO);
}

// 测试各等级打印分支与默认分支
TEST(UtilModules_CLog, PrintAllLevels)
{
    CLog& log = CLog::GetInstance();
    log.SetLevel(CLOG_LEVEL_DEBUG);

    // 各等级均不崩溃（覆盖 switch 各分支）
    log.Print(CLOG_LEVEL_DEBUG, __LINE__, "TAG", "debug %d\n", 1);
    log.Print(CLOG_LEVEL_INFO,  __LINE__, "TAG", "info %d\n", 2);
    log.Print(CLOG_LEVEL_WARN,  __LINE__, "TAG", "warn %d\n", 3);
    log.Print(CLOG_LEVEL_ERROR, __LINE__, "TAG", "error %d\n", 4);
    log.Print(CLOG_LEVEL_BUTT,  __LINE__, "TAG", "unknown %d\n", 5);

    log.SetLevel(CLOG_LEVEL_INFO);
}

// 测试打印等级过滤（低等级被拦截）
TEST(UtilModules_CLog, LevelFilterBlocksLowerLevel)
{
    CLog& log = CLog::GetInstance();
    log.SetLevel(CLOG_LEVEL_ERROR);

    // DEBUG/INFO/WARN 低于 ERROR，应被拦截（覆盖过滤分支）
    log.Print(CLOG_LEVEL_DEBUG, __LINE__, "TAG", "filtered debug\n");
    log.Print(CLOG_LEVEL_INFO,  __LINE__, "TAG", "filtered info\n");
    log.Print(CLOG_LEVEL_WARN,  __LINE__, "TAG", "filtered warn\n");

    log.SetLevel(CLOG_LEVEL_INFO);
}

// 测试注册回调后打印走回调分支
TEST(UtilModules_CLog, RegisterPrintCallback)
{
    CLog& log = CLog::GetInstance();

    std::atomic<int32_t> hitLevel(0);
    std::string hitTag;
    log.RegisterPrintCallback([&](int level, int line, const char* tag, const char* fmt, va_list args) {
        hitLevel.store(level);
        hitTag = tag;
    });

    log.Print(CLOG_LEVEL_INFO, __LINE__, "CB_TAG", "hello %d\n", 42);
    EXPECT_EQ(hitLevel.load(), CLOG_LEVEL_INFO);
    EXPECT_EQ(hitTag, "CB_TAG");

    // 清空回调，恢复默认打印
    log.RegisterPrintCallback(nullptr);
}
