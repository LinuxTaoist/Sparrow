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
#include "gtest/gtest.h"
#include "RunningTiming.h"
#include <unistd.h>

// 测试构造函数是否自动启动计时
TEST(Util_RunningTiming, ConstructorStartsTiming) {
    RunningTiming timer;
    uint64_t elapsedTime = timer.GetElapsedTimeInMSec();
    EXPECT_GE(elapsedTime, (uint64_t)(0));
    EXPECT_LE(elapsedTime, (uint64_t)(0 + 5));  // 误差在5毫秒以内
}

// 测试GetElapsedTimeInMSec 误差10ms
TEST(Util_RunningTiming, StartUpdatesStartTime) {
    uint64_t diffMS = 10;
    RunningTiming timer1;
    usleep(100 * 1000); // 休眠 100 毫秒
    uint64_t elapsed1 = timer1.GetElapsedTimeInMSec();
    EXPECT_GE(elapsed1, (uint64_t)(100 - diffMS));
    EXPECT_LE(elapsed1, (uint64_t)(100 + diffMS));

    {
        RunningTiming timer2;
        usleep(50 * 1000); // 休眠 50 毫秒
        uint64_t elapsed2 = timer2.GetElapsedTimeInMSec();
        EXPECT_GT(elapsed2, (uint64_t)(50 - diffMS));
        EXPECT_LT(elapsed2, (uint64_t)(50 + diffMS));
    }

    usleep(50 * 1000);     // 休眠 50 毫秒
    uint64_t elapsed3 = timer1.GetElapsedTimeInMSec();
    EXPECT_GE(elapsed3, (uint64_t)(200 - diffMS));
    EXPECT_LE(elapsed3, (uint64_t)(200 + diffMS));
}

// 测试 GetElapsedTimeInSec 误差1s
TEST(Util_RunningTiming, GetElapsedTimeInSecCalculatesCorrectly) {
    uint64_t diffSec = 1;
    RunningTiming timer;
    usleep(500 * 1000); // 休眠 0.5 秒
    uint64_t elapsedSec = timer.GetElapsedTimeInSec();
    EXPECT_GE(elapsedSec, (uint64_t)(0));
    EXPECT_LE(elapsedSec, (uint64_t)(0 + diffSec));

    {
        RunningTiming timer2;
        usleep(2 * 1000 * 1000); // 休眠 2 秒
        uint64_t elapsedSec2 = timer2.GetElapsedTimeInSec();
        EXPECT_GE(elapsedSec2, (uint64_t)(2 - diffSec));
        EXPECT_LE(elapsedSec2, (uint64_t)(2 + diffSec));
    }

    usleep(1 * 1000 * 1000); // 休眠 1 秒
    uint64_t elapsedSec3 = timer.GetElapsedTimeInSec();
    EXPECT_GE(elapsedSec3, (uint64_t)(3 - diffSec));
    EXPECT_LE(elapsedSec3, (uint64_t)(3 + diffSec));
}
