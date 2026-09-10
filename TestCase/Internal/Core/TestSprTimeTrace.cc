/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprTimeTrace.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SprTimeTrace 系统启动耗时追踪内部测试
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
#include <string>
#include "SprTimeTrace.h"
#include "gtest/gtest.h"

// 测试单例模式返回同一实例
TEST(Core_SprTimeTrace, SingletonReturnsSameInstance)
{
    SprTimeTrace* a = SprTimeTrace::GetInstance();
    SprTimeTrace* b = SprTimeTrace::GetInstance();
    ASSERT_TRUE(a != nullptr);
    EXPECT_EQ(a, b);
}

// 测试记录多个时间追踪点（系统各入口耗时打点）
TEST(Core_SprTimeTrace, TimeTracePointRecordsEntries)
{
    SprTimeTrace* trace = SprTimeTrace::GetInstance();
    ASSERT_TRUE(trace != nullptr);

    // 模拟系统启动各阶段打点，不崩溃且可重复调用
    trace->TimeTracePoint(0, "boot_start");
    trace->TimeTracePoint(1, "load_config");
    trace->TimeTracePoint(2, "init_binder");
    trace->TimeTracePoint(3, "start_services");

    // 相同 id 重复打点也应正常
    trace->TimeTracePoint(0, "boot_start_again");
}
