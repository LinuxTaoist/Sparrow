/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestRunningTiming.cc
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
#include <gtest/gtest.h>
#include "RunningTiming.h"
#include <unistd.h>

// 测试构造函数是否自动启动计时
TEST(Util_RunningTiming, ConstructorStartsTiming) {
    RunningTiming timer;
    uint64_t elapsedTime = timer.GetElapsedTimeInMSec();
    EXPECT_GE(elapsedTime, static_cast<uint64_t>(0));
}

// 间接测试 Start 方法是否正确更新开始时间
TEST(Util_RunningTiming, StartUpdatesStartTime) {
    RunningTiming timer1;
    usleep(100 * 1000); // 休眠 100 毫秒
    uint64_t elapsed1 = timer1.GetElapsedTimeInMSec();

    {
        RunningTiming timer2;
        usleep(50 * 1000); // 休眠 50 毫秒
        uint64_t elapsed2 = timer2.GetElapsedTimeInMSec();
        EXPECT_GT(elapsed1, elapsed2);
    }
}

// 测试 GetElapsedTimeInSec 方法计算秒数的正确性
TEST(Util_RunningTiming, GetElapsedTimeInSecCalculatesCorrectly) {
    RunningTiming timer;
    usleep(1 * 1000 * 1000); // 休眠 1 秒
    uint64_t elapsedSec = timer.GetElapsedTimeInSec();
    EXPECT_GE(elapsedSec, static_cast<uint64_t>(1));
}

// 测试 GetElapsedTimeInMSec 方法计算毫秒数的正确性
TEST(Util_RunningTiming, GetElapsedTimeInMSecCalculatesCorrectly) {
    RunningTiming timer;
    usleep(100 * 1000); // 休眠 100 毫秒
    uint64_t elapsedMSec = timer.GetElapsedTimeInMSec();
    EXPECT_GE(elapsedMSec, static_cast<uint64_t>(100));
}
