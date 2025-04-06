/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestProcMutex.cc
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
#include <thread>
#include <atomic>
#include <fcntl.h>
#include <sys/mman.h>
#include "ProcMutex.h"
#include "gtest/gtest.h"

const std::string mutexName = "test_mutex";

// 测试 ProcMutex 构造函数
TEST(Util_ProcMutex, Constructor) {
    ProcMutex mutex(mutexName);
    std::mutex stdMutex;
    ProcLockGuard guard(mutex, stdMutex);
}

// 测试 ProcMutex 析构函数
TEST(Util_ProcMutex, Destructor) {
    {
        ProcMutex mutex(mutexName);
    }
    ProcMutex newMutex(mutexName);
    std::mutex stdMutex;
    ProcLockGuard guard(newMutex, stdMutex);
}

// 测试 ProcMutex 的 Lock 和 Unlock 方法
TEST(Util_ProcMutex, LockAndUnlock) {
    ProcMutex mutex(mutexName);
    std::mutex stdMutex;
    {
        ProcLockGuard guard(mutex, stdMutex);
    }
    {
        ProcLockGuard guard(mutex, stdMutex);
    }
}

// 测试 ProcMutex 多次加锁解锁
TEST(Util_ProcMutex, MultipleLockAndUnlock) {
    ProcMutex mutex(mutexName);
    std::mutex stdMutex;
    const int times = 10;
    for (int i = 0; i < times; ++i) {
        ProcLockGuard guard(mutex, stdMutex);
    }
}

// 测试 ProcLockGuard 的 RAII 特性
TEST(Util_ProcMutex, RAIIFeature) {
    ProcMutex mutex(mutexName);
    std::mutex stdMutex;
    {
        ProcLockGuard guard(mutex, stdMutex);
    }
    {
        ProcLockGuard guard(mutex, stdMutex);
    }
}

// 测试多线程环境下 ProcMutex 的功能
TEST(Util_ProcMutex, MultiThreading) {
    ProcMutex mutex(mutexName);
    std::mutex stdMutex;
    int counter = 0;
    const int numThreads = 10;
    const int numIterations = 1000;
    std::vector<std::thread> threads;

    auto worker = [&mutex, &stdMutex, &counter, numIterations]() {
        for (int i = 0; i < numIterations; ++i) {
            // 注释guard，此case大概率会失败
            ProcLockGuard guard(mutex, stdMutex);
            std::this_thread::sleep_for(std::chrono::microseconds(3));
            counter++;
        }
    };

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(counter, numThreads * numIterations);
}

// 测试多进程环境下 ProcMutex 的功能
TEST(Util_ProcMutex, MultiProcess) {
    ProcMutex mutex(mutexName);
    std::mutex stdMutex;
    const int numChildren = 10;
    const int numIterations = 1000;

    // 创建共享内存对象
    int shmFd = shm_open("/test_shared_mem", O_CREAT | O_RDWR, 0666);
    if (shmFd == -1) {
        perror("shm_open");
        FAIL();
    }

    // 设置共享内存大小
    if (ftruncate(shmFd, sizeof(int)) == -1) {
        perror("ftruncate");
        close(shmFd);
        shm_unlink("/test_shared_mem");
        FAIL();
    }

    // 映射共享内存
    int* shared_counter = static_cast<int*>(mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0));
    if (shared_counter == MAP_FAILED) {
        perror("mmap");
        close(shmFd);
        shm_unlink("/test_shared_mem");
        FAIL();
    }

    *shared_counter = 0;
    for (int i = 0; i < numChildren; ++i) {
        pid_t pid = fork();
        if (pid == 0) {     // 子进程
            for (int j = 0; j < numIterations; ++j) {
                // 注释guard，此case大概率会失败
                ProcLockGuard guard(mutex, stdMutex);
                std::this_thread::sleep_for(std::chrono::microseconds(3));
                (*shared_counter)++;
            }
            _exit(0);
        } else if (pid < 0) {
            perror("fork");
            munmap(shared_counter, sizeof(int));
            close(shmFd);
            shm_unlink("/test_shared_mem");
            FAIL();
        }
    }

    // 等待所有子进程结束
    for (int i = 0; i < numChildren; ++i) {
        int status;
        wait(&status);
    }

    // 验证计数器值
    EXPECT_EQ(*shared_counter, numChildren * numIterations);

    // 清理共享内存
    munmap(shared_counter, sizeof(int));
    close(shmFd);
    shm_unlink("/test_shared_mem");
}
