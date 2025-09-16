/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprThreadPool.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/04/24
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/04/24 | 1.0.0.1   | Xiang.D        | Fix return type issues of SubmitTask
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <atomic>
#include <string>
#include <vector>
#include <thread>
#include <unistd.h>
#include <sys/time.h>
#include "SprThreadPool.h"
#include "gtest/gtest.h"

// 工具函数：获取当前时间（微秒）
static uint64_t GetCurrentTimeUs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

// 工具函数：等待条件满足（带超时，单位：微秒），返回true=条件满足，false=超时
template <typename ConditionFunc>
static bool WaitWithTimeout(ConditionFunc condition, uint64_t timeoutUs) {
    uint64_t startUs = GetCurrentTimeUs();
    while (!condition()) {
        if (GetCurrentTimeUs() - startUs > timeoutUs) {
            return false;
        }
        usleep(20000);  // 20ms轮询一次
    }
    return true;
}

// 测试1：单例模式唯一性
TEST(Core_SprThreadPool, SingletonInstanceReturnsSameObject) {
    SprThreadPool* instance1 = SprThreadPool::GetInstance(2);
    SprThreadPool* instance2 = SprThreadPool::GetInstance(4);
    SprThreadPool* instance3 = SprThreadPool::GetInstance();

    // 验证实例非空且地址相同
    ASSERT_TRUE(instance1 != NULL) << "First instance is null";
    ASSERT_EQ(instance1, instance2) << "Second instance address mismatch";
    ASSERT_EQ(instance1, instance3) << "Third instance address mismatch";

    // 验证初始化参数仅首次生效
    EXPECT_EQ(instance2->GetTotalWorkerCount(), 2) << "Initial param takes effect on subsequent calls";
}

// 测试2：无返回值任务基础功能
TEST(Core_SprThreadPool, SubmitTaskNoReturnExecutesWithoutLoss) {
    SprThreadPool* pool = SprThreadPool::GetInstance(2);
    ASSERT_TRUE(pool != NULL) << "Thread pool instance is null";

    std::atomic<int> taskCount(0);
    const int kTestTaskNum = 5;
    const uint64_t kTimeoutUs = 1000000;  // 1秒超时

    // 提交5个无返回值任务
    for (int i = 0; i < kTestTaskNum; ++i) {
        pool->SubmitTask([&taskCount]() {
            usleep(10000);  // 10ms模拟任务耗时
            taskCount++;
        });
    }

    // 等待任务计数达到预期
    bool isTimeout = !WaitWithTimeout([&taskCount, kTestTaskNum]() {
        return taskCount == kTestTaskNum;
    }, kTimeoutUs);
    ASSERT_FALSE(isTimeout) << "No-return task timeout (count=" << taskCount << ")";
    EXPECT_EQ(taskCount, kTestTaskNum) << "Task count does not match expected";
}

// 测试3：通过共享变量获取任务结果（替代带返回值任务）
TEST(Core_SprThreadPool, TaskResultViaSharedVariable) {
    SprThreadPool* pool = SprThreadPool::GetInstance(2);
    ASSERT_TRUE(pool != NULL) << "Thread pool instance is null";

    const int kA = 32;
    const int kB = 48;
    const int kExpectResult = kA + kB;
    std::atomic<int> actualResult(0);
    std::atomic<bool> taskDone(false);
    const uint64_t kTimeoutUs = 1000000;

    // 提交计算任务，通过共享变量返回结果
    pool->SubmitTask([kA, kB, &actualResult, &taskDone]() {
        usleep(10000);  // 10ms模拟任务耗时
        actualResult = kA + kB;
        taskDone = true;
    });

    // 等待任务完成
    bool isTimeout = !WaitWithTimeout([&taskDone]() {
        return taskDone.load();
    }, kTimeoutUs);
    ASSERT_FALSE(isTimeout) << "Task result timeout";
    EXPECT_EQ(actualResult, kExpectResult) << "Task result does not match expected";
}

// 测试4：字符串结果通过共享变量返回
TEST(Core_SprThreadPool, StringResultViaSharedVariable) {
    SprThreadPool* pool = SprThreadPool::GetInstance(2);
    ASSERT_TRUE(pool != NULL) << "Thread pool instance is null";

    const std::string kStr1 = "Spr";
    const std::string kStr2 = "ThreadPool";
    const std::string kExpectResult = kStr1 + "-" + kStr2;
    std::string actualResult;
    std::atomic<bool> taskDone(false);
    const uint64_t kTimeoutUs = 1000000;

    // 使用互斥锁保护字符串访问
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    // 提交字符串处理任务
    pool->SubmitTask([kStr1, kStr2, &actualResult, &taskDone, &mutex]() {
        usleep(10000);  // 10ms模拟任务耗时
        std::string result = kStr1 + "-" + kStr2;

        // 加锁保护共享字符串
        pthread_mutex_lock(&mutex);
        actualResult = result;
        pthread_mutex_unlock(&mutex);

        taskDone = true;
    });

    // 等待任务完成
    bool isTimeout = !WaitWithTimeout([&taskDone]() {
        return taskDone.load();
    }, kTimeoutUs);
    ASSERT_FALSE(isTimeout) << "String task timeout";

    // 加锁读取结果
    pthread_mutex_lock(&mutex);
    std::string resultCopy = actualResult;
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);

    EXPECT_EQ(resultCopy, kExpectResult) << "String result does not match expected";
}

// 测试5：带自定义结构体参数的任务
TEST(Core_SprThreadPool, SubmitTaskWithStructParamPassesCorrectly) {
    SprThreadPool* pool = SprThreadPool::GetInstance(2);
    ASSERT_TRUE(pool != NULL) << "Thread pool instance is null";

    // 定义自定义结构体
    struct TestStruct {
        int id;
        std::string name;
        bool isValid;
        uint64_t timestamp;
    };
    TestStruct input = {1001, "StructParamTest", true, GetCurrentTimeUs()};
    TestStruct output = {0, "", false, 0};
    std::atomic<bool> taskDone(false);
    const uint64_t kTimeoutUs = 1000000;

    // 使用互斥锁保护结构体访问
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    // 提交带结构体参数的任务
    pool->SubmitTask([input, &output, &taskDone, &mutex]() {
        usleep(10000);

        // 加锁保护共享结构体
        pthread_mutex_lock(&mutex);
        output.id = input.id;
        output.name = input.name;
        output.isValid = input.isValid;
        output.timestamp = input.timestamp;
        pthread_mutex_unlock(&mutex);

        taskDone = true;
    });

    // 等待任务完成
    bool isTimeout = !WaitWithTimeout([&taskDone]() {
        return taskDone.load();
    }, kTimeoutUs);
    ASSERT_FALSE(isTimeout) << "Struct-param task timeout";

    // 加锁读取结果
    pthread_mutex_lock(&mutex);
    TestStruct outputCopy = output;
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);

    EXPECT_EQ(outputCopy.id, input.id) << "Struct id mismatch";
    EXPECT_EQ(outputCopy.name, input.name) << "Struct name mismatch";
    EXPECT_EQ(outputCopy.isValid, input.isValid) << "Struct isValid mismatch";
    EXPECT_EQ(outputCopy.timestamp, input.timestamp) << "Struct timestamp mismatch";
}

// 测试6：初始线程数控制
TEST(Core_SprThreadPool, InitialThreadCountMatchesConfig) {
    SprThreadPool* pool = SprThreadPool::GetInstance(2);
    ASSERT_TRUE(pool != NULL) << "Thread pool instance is null";

    const uint64_t kTimeoutUs = 500000;  // 500ms等待线程初始化

    // 验证初始总线程数
    EXPECT_EQ(pool->GetTotalWorkerCount(), 2) << "Initial total thread count mismatch (expected 2)";

    // 验证初始空闲线程数
    bool isTimeout = !WaitWithTimeout([&pool]() {
        return pool->GetIdleWorkerCount() == 2;
    }, kTimeoutUs);
    ASSERT_FALSE(isTimeout) << "Initial idle thread count timeout";
    EXPECT_EQ(pool->GetIdleWorkerCount(), 2) << "Initial idle thread count mismatch (expected 2)";

    // 提交1个任务，验证空闲线程数减少
    std::atomic<bool> taskDone(false);
    pool->SubmitTask([&taskDone]() {
        usleep(50000);  // 50ms任务
        taskDone = true;
    });

    isTimeout = !WaitWithTimeout([&pool]() {
        return pool->GetIdleWorkerCount() == 1;
    }, kTimeoutUs);
    ASSERT_FALSE(isTimeout) << "Idle count down timeout";
    EXPECT_EQ(pool->GetIdleWorkerCount(), 1) << "Idle count not down after task submit";

    // 任务完成后，验证空闲线程数恢复
    isTimeout = !WaitWithTimeout([&taskDone]() {
        return taskDone.load();
    }, kTimeoutUs);
    ASSERT_FALSE(isTimeout) << "Task done timeout";

    isTimeout = !WaitWithTimeout([&pool]() {
        return pool->GetIdleWorkerCount() == 2;
    }, kTimeoutUs);
    ASSERT_FALSE(isTimeout) << "Idle count recover timeout";
    EXPECT_EQ(pool->GetIdleWorkerCount(), 2) << "Idle count not recover after task done";
}

// 测试7：线程池满时任务排队
TEST(Core_SprThreadPool, TasksQueueNormallyWhenPoolIsFull) {
    SprThreadPool* pool = SprThreadPool::GetInstance(2);
    ASSERT_TRUE(pool != NULL) << "Thread pool instance is null";

    const int kMaxWorkers = SPR_THREAD_POOL_MAX_WORKERS;
    const int kTestTaskNum = kMaxWorkers + 5;  // 超过最大线程数5个任务
    std::atomic<int> taskCount(0);
    const uint64_t kTimeoutUs = 3000000;  // 3秒超时

    // 提交大量耗时任务
    for (int i = 0; i < kTestTaskNum; ++i) {
        pool->SubmitTask([&taskCount]() {
            usleep(100000);  // 100ms模拟任务耗时
            taskCount++;
        });
    }

    // 验证任务执行过程中，总线程数不超过最大值
    bool countCheckTimeout = !WaitWithTimeout([&taskCount]() {
        return taskCount > 0;  // 至少有一个任务开始执行
    }, kTimeoutUs / 3);
    if (!countCheckTimeout) {
        EXPECT_LE(pool->GetTotalWorkerCount(), kMaxWorkers) << "Thread count exceeds max limit";
    }

    // 等待所有任务完成
    bool isTimeout = !WaitWithTimeout([&taskCount, kTestTaskNum]() {
        return taskCount == kTestTaskNum;
    }, kTimeoutUs);
    ASSERT_FALSE(isTimeout) << "Pool-full task timeout (count=" << taskCount << ")";
    EXPECT_EQ(taskCount, kTestTaskNum) << "Tasks lost when pool is full";
}

// 测试8：并发提交任务
TEST(Core_SprThreadPool, ConcurrentSubmitTasksWithoutRace) {
    SprThreadPool* pool = SprThreadPool::GetInstance(4);
    ASSERT_TRUE(pool != NULL) << "Thread pool instance is null";

    const int kSubmitThreadNum = 3;   // 3个线程同时提交任务
    const int kTaskPerThread = 10;    // 每个线程提交10个任务
    const int kTotalTaskNum = kSubmitThreadNum * kTaskPerThread;
    std::atomic<int> taskCount(0);
    std::vector<std::thread> submitThreads;
    const uint64_t kTimeoutUs = 2000000;  // 2秒超时

    // 启动多个线程并发提交任务
    for (int i = 0; i < kSubmitThreadNum; ++i) {
        submitThreads.emplace_back([&taskCount, kTaskPerThread, pool]() {
            for (int j = 0; j < kTaskPerThread; ++j) {
                pool->SubmitTask([&taskCount]() {
                    usleep(5000);  // 5ms模拟任务耗时
                    taskCount++;
                });
                usleep(1000);  // 1ms间隔
            }
        });
    }

    // 等待所有提交线程结束
    for (auto& thread : submitThreads) {
        thread.join();
    }

    // 等待所有任务执行完成
    bool isTimeout = !WaitWithTimeout([&taskCount, kTotalTaskNum]() {
        return taskCount == kTotalTaskNum;
    }, kTimeoutUs);
    ASSERT_FALSE(isTimeout) << "Concurrent submit timeout (count=" << taskCount << ")";
    EXPECT_EQ(taskCount, kTotalTaskNum) << "Tasks lost in concurrent submit";
}

// 测试9：长时间运行任务的线程占用
TEST(Core_SprThreadPool, LongRunningTaskOccupiesThreadCorrectly) {
    SprThreadPool* pool = SprThreadPool::GetInstance(2);
    ASSERT_TRUE(pool != NULL) << "Thread pool instance is null";

    std::atomic<bool> longTaskDone(false);
    const uint64_t kLongTaskDurationUs = 300000;  // 300ms长时间任务
    const uint64_t kTimeoutUs = 500000;          // 500ms超时

    // 提交1个长时间运行的任务
    pool->SubmitTask([&longTaskDone, kLongTaskDurationUs]() {
        usleep(kLongTaskDurationUs);
        longTaskDone = true;
    });

    // 等待任务开始执行，验证空闲线程数减少
    bool isTimeout = !WaitWithTimeout([&pool]() {
        return pool->GetIdleWorkerCount() == 1;
    }, kTimeoutUs / 2);
    ASSERT_FALSE(isTimeout) << "Long task thread occupy timeout";
    EXPECT_EQ(pool->GetIdleWorkerCount(), 1) << "Long task not occupy thread";

    // 任务未完成前，提交第二个任务
    std::atomic<bool> shortTaskDone(false);
    pool->SubmitTask([&shortTaskDone]() {
        usleep(50000);  // 50ms短任务
        shortTaskDone = true;
    });

    isTimeout = !WaitWithTimeout([&pool]() {
        return pool->GetIdleWorkerCount() == 0;
    }, kTimeoutUs / 2);
    ASSERT_FALSE(isTimeout) << "Short task thread occupy timeout";
    EXPECT_EQ(pool->GetIdleWorkerCount(), 0) << "Short task not occupy thread";

    // 等待短任务完成，验证空闲线程数恢复
    isTimeout = !WaitWithTimeout([&shortTaskDone]() {
        return shortTaskDone.load();
    }, kTimeoutUs / 2);
    ASSERT_FALSE(isTimeout) << "Short task done timeout";

    isTimeout = !WaitWithTimeout([&pool]() {
        return pool->GetIdleWorkerCount() == 1;
    }, kTimeoutUs / 2);
    ASSERT_FALSE(isTimeout) << "Idle count recover after short task timeout";
    EXPECT_EQ(pool->GetIdleWorkerCount(), 1) << "Idle count not recover after short task";

    // 等待长任务完成，验证空闲线程数恢复
    isTimeout = !WaitWithTimeout([&longTaskDone]() {
        return longTaskDone.load();
    }, kTimeoutUs);
    ASSERT_FALSE(isTimeout) << "Long task done timeout";

    isTimeout = !WaitWithTimeout([&pool]() {
        return pool->GetIdleWorkerCount() == 2;
    }, kTimeoutUs / 2);
    ASSERT_FALSE(isTimeout) << "Idle count recover after long task timeout";
    EXPECT_EQ(pool->GetIdleWorkerCount(), 2) << "Idle count not recover after long task";
}

// 测试10：池析构前任务完成
TEST(Core_SprThreadPool, TasksCompleteBeforePoolDestruction) {
    // 局部作用域模拟池生命周期
    {
        SprThreadPool* pool = SprThreadPool::GetInstance(2);
        ASSERT_TRUE(pool != NULL) << "Thread pool instance is null";

        std::atomic<int> taskCount(0);
        const int kTestTaskNum = 4;
        const uint64_t kTimeoutUs = 1000000;

        // 提交任务后，等待所有任务完成
        for (int i = 0; i < kTestTaskNum; ++i) {
            pool->SubmitTask([&taskCount]() {
                usleep(10000);  // 10ms模拟任务耗时
                taskCount++;
            });
        }

        bool isTimeout = !WaitWithTimeout([&taskCount, kTestTaskNum]() {
            return taskCount == kTestTaskNum;
        }, kTimeoutUs);
        ASSERT_FALSE(isTimeout) << "Task before destruction timeout";
        EXPECT_EQ(taskCount, kTestTaskNum) << "Tasks incomplete before pool destruction";
    }  // 此处池析构

    // 析构后重新获取实例，验证单例可正常复用
    SprThreadPool* newPool = SprThreadPool::GetInstance(2);
    ASSERT_TRUE(newPool != NULL) << "Pool unavailable after destruction";
    EXPECT_EQ(newPool->GetTotalWorkerCount(), 2) << "Pool reinit total count mismatch";

    // 复用实例提交任务
    std::atomic<bool> reuseTaskDone(false);
    newPool->SubmitTask([&reuseTaskDone]() {
        usleep(10000);
        reuseTaskDone = true;
    });

    bool isTimeout = !WaitWithTimeout([&reuseTaskDone]() {
        return reuseTaskDone.load();
    }, 1000000);
    ASSERT_FALSE(isTimeout) << "Reuse task timeout";
    EXPECT_TRUE(reuseTaskDone) << "Pool not reusable after destruction";
}
