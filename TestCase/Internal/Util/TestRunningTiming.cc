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
#include <atomic>
#include <thread>
#include <unistd.h>
#include "gtest/gtest.h"
#include "RunningTiming.h"

#define LOG_TAG "TestRTiming"

// 测试构造函数是否自动启动计时
TEST(Util_RunningTiming, ConstructorStartsTiming) {
    RunningTiming timer;
    uint64_t elapsedTime = timer.GetElapsedTimeInMSec();
    EXPECT_GE(elapsedTime, (uint64_t)(0));
    EXPECT_LE(elapsedTime, (uint64_t)(0 + 5));  // 误差在5毫秒以内
}

TEST(Util_RunningTiming, NormalUsing) {
    uint64_t totalTimer = 0;
    for (int i = 0; i < 10; ++i) {
        RunningTiming timer;
        usleep(100 * 1000); // 休眠 100 毫秒
        uint64_t elapsedTime = timer.GetElapsedTimeInMSec();
        totalTimer += elapsedTime;
        EXPECT_GE(elapsedTime, (uint64_t)(100));
        EXPECT_LE(elapsedTime, (uint64_t)(100 + 5));  // 误差在5毫秒以内
    }

    std::cout << "elapsedTime: " << totalTimer / 10 << " ms - " << 100 << std::endl;
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

// 测试多线程环境下每个线程的计时器是否独立工作
TEST(Util_RunningTiming, MultiThreadIndependentMeasurement) {
    const int numThreads = 5;
    std::vector<std::thread> threads;
    std::vector<uint64_t> measuredTimes(numThreads);
    uint64_t diffMS = 10; // 允许的误差范围（毫秒）

    // 创建多个线程，每个线程有不同的睡眠时间
    for (int i = 0; i < numThreads; ++i) {
        uint64_t sleepTimeMs = (i + 1) * 100; // 线程0睡眠100ms，线程1睡眠200ms，依此类推
        threads.emplace_back([&, i, sleepTimeMs]() {
            RunningTiming timer;
            usleep(sleepTimeMs * 1000); // 将毫秒转换为微秒
            measuredTimes[i] = timer.GetElapsedTimeInMSec();
            std::cout << "Thread " << i << ": expectedTimeMs: " << sleepTimeMs << ", measuredTime: " << measuredTimes[i] << std::endl;
        });
    }

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    // 验证每个线程测量的时间是否接近预期
    for (int i = 0; i < numThreads; ++i) {
        uint64_t expectedTimeMs = (i + 1) * 100;
        EXPECT_GE(measuredTimes[i], expectedTimeMs - diffMS);
        EXPECT_LE(measuredTimes[i], expectedTimeMs + diffMS);
    }
}

// 测试多个计时器在同一个线程中的使用
TEST(Util_RunningTiming, MultipleTimersInSingleThread) {
    RunningTiming timer1, timer2;
    uint64_t diffMS = 10; // 允许的误差范围（毫秒）

    // 第一个计时器计时第一段代码
    usleep(150 * 1000); // 150毫秒
    uint64_t time1 = timer1.GetElapsedTimeInMSec();

    // 第二个计时器计时第二段代码
    usleep(200 * 1000); // 200毫秒
    uint64_t time2 = timer2.GetElapsedTimeInMSec();

    // 第一个计时器继续计时第三段代码
    usleep(100 * 1000); // 100毫秒
    uint64_t time3 = timer1.GetElapsedTimeInMSec();

    // 验证计时结果
    EXPECT_GE(time1, 150 - diffMS);
    EXPECT_LE(time1, 150 + diffMS);

    EXPECT_GE(time2, 350 - diffMS);
    EXPECT_LE(time2, 350 + diffMS);

    EXPECT_GE(time3, 450 - diffMS);
    EXPECT_LE(time3, 450 + diffMS);
}

// 测试在多线程环境下同时启动计时器的准确性
TEST(Util_RunningTiming, ConcurrentStartTimers) {
    const int numThreads = 10;
    std::vector<std::thread> threads;
    std::atomic<bool> startFlag(false);
    std::vector<uint64_t> measuredTimes(numThreads);
    uint64_t diffMS = 15; // 增加误差范围以应对线程同步开销

    // 创建多个线程，等待同一个信号同时启动计时器
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, i]() {
            // 等待启动信号
            while (!startFlag.load()) {
                // 移除了std::this_thread::yield()，使用短暂休眠代替
                usleep(1000); // 1毫秒
            }

            RunningTiming timer;
            usleep(200 * 1000); // 200毫秒
            measuredTimes[i] = timer.GetElapsedTimeInMSec();
        });
    }

    // 让所有线程准备好
    usleep(100 * 1000); // 100毫秒

    // 同时启动所有计时器
    startFlag.store(true);

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    // 验证所有计时器测量的时间是否接近预期
    for (int i = 0; i < numThreads; ++i) {
        EXPECT_GE(measuredTimes[i], 200 - diffMS);
        EXPECT_LE(measuredTimes[i], 200 + diffMS);
    }
}

// 测试在多个线程中使用同一个计时器实例（预期失败，因为计时器非线程安全）
TEST(Util_RunningTiming, SharedTimerInMultipleThreads) {
    RunningTiming sharedTimer;
    const int numThreads = 5;
    std::vector<std::thread> threads;
    std::atomic<bool> startFlag(false);
    std::atomic<bool> stopFlag(false);
    uint64_t diffMS = 20; // 由于竞争条件，误差范围较大

    // 每个线程都会尝试使用同一个计时器实例
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&]() {
            // 等待启动信号
            while (!startFlag.load()) {
                std::this_thread::yield();
            }

            // 模拟线程执行不同时长的任务
            uint64_t sleepTimeMs = (50 + rand() % 100); // 50-150毫秒
            usleep(sleepTimeMs * 1000); // 50-150毫秒

            // 记录时间（注意：多个线程同时访问同一个计时器实例）
            uint64_t thTimer = sharedTimer.GetElapsedTimeInMSec();
            EXPECT_GE(thTimer, sleepTimeMs - diffMS);
            EXPECT_LE(thTimer, sleepTimeMs + diffMS);

            // 再次等待，确保所有线程完成前stopFlag未被设置
            while (!stopFlag.load()) {
               std::this_thread::yield();
            }
        });
    }

    // 启动所有线程
    startFlag.store(true);

    // 等待一段时间后设置停止标志
    usleep(200 * 1000); // 200毫秒
    stopFlag.store(true);

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    // 验证计时器总时间
    uint64_t totalTime = sharedTimer.GetElapsedTimeInMSec();
    std::cout << "Final shared timer value: " << totalTime << " ms" << std::endl;

    // 注意：由于多个线程同时访问同一个计时器实例，这个测试可能会失败
    // 这验证了RunningTiming类不是线程安全的，不应该在多线程中共享同一个实例
    EXPECT_GE(totalTime, 200 - diffMS);
    EXPECT_LE(totalTime, 200 + diffMS);
}
