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
#include "SprLog.h"
#include "ProcMutex.h"
#include "gtest/gtest.h"

#define LOG_TAG "TestPMutex"

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
            usleep(300);
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
                usleep(300);
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

// 测试持锁进程异常退出时，其他进程是否能正常获取锁
TEST(Util_ProcMutex, MultiProcessCrashRecovery) {
    const std::string DEMO_SHARED_MUTEX = "demo_shared_mutex";
    const int NUM_PROCESSES = 3;         // Total number of processes
    const int CRASH_PROCESS_ID = 1;      // ID of the process that will crash
    const int MAX_ITERATIONS = 3;        // Maximum number of lock acquisition attempts per process

    // Define shared memory structure to track access statistics
    struct SharedData {
        int access_count[NUM_PROCESSES]; // Access count for each process
        int total_accesses;              // Total access count across all processes
    };

    // Create shared memory
    int shm_fd = shm_open("/test_crash_recovery_shm", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        SPR_LOGE("shm_open failed");
        FAIL() << "Failed to create shared memory";
    }

    // Set shared memory size
    if (ftruncate(shm_fd, sizeof(SharedData)) == -1) {
        perror("ftruncate failed");
        close(shm_fd);
        shm_unlink("/test_crash_recovery_shm");
        FAIL() << "Failed to set shared memory size";
    }

    // Map shared memory
    SharedData* shared_data = static_cast<SharedData*>(
        mmap(nullptr, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0)
    );
    if (shared_data == MAP_FAILED) {
        perror("mmap failed");
        close(shm_fd);
        shm_unlink("/test_crash_recovery_shm");
        FAIL() << "Failed to map shared memory";
    }

    // Initialize shared data
    memset(shared_data, 0, sizeof(SharedData));

    // Child process work function as lambda
    auto childProcessWork = [&](int processId, bool isCrashProcess) {
        std::mutex threadMutex;
        ProcMutex procMutex(DEMO_SHARED_MUTEX);
        SPR_LOGD("Process %d: Starting, preparing to compete for lock...", processId);

        // Loop to attempt accessing shared resource
        for (int i = 0; i < MAX_ITERATIONS; ++i) {
            ProcLockGuard lockGuard(procMutex, threadMutex);

            // Record access counts
            shared_data->access_count[processId - 1]++;
            shared_data->total_accesses++;
            SPR_LOGD("Process %d: Successfully acquired lock, accessing resource (attempt %d, total accesses: %d)",
                   processId, i + 1, shared_data->total_accesses);

            // Simulate processing time
            usleep(500000);

            // Specific process crashes while holding lock (simulate crash)
            if (isCrashProcess && i == 1) {
                SPR_LOGD("Process %d: About to exit abnormally (while holding lock)!", processId);
                std::abort(); // Abnormal exit without releasing lock
            }

            SPR_LOGD("Process %d: Released lock, waiting for next attempt...", processId);
        }

        _exit(EXIT_SUCCESS);
    };

    // Create multiple child processes
    std::vector<pid_t> child_pids;
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            // Clean up already created child processes
            for (pid_t p : child_pids) {
                kill(p, SIGKILL);
            }
            FAIL() << "Failed to create child process";
        }
        else if (pid == 0) {
            // Child process executes work function
            childProcessWork(i + 1, (i + 1 == CRASH_PROCESS_ID));
            _exit(EXIT_SUCCESS); // Normal exit
        }
        else {
            child_pids.push_back(pid);
            usleep(50000);
        }
    }

    // Wait for all child processes to finish
    int crash_process_exit_status = -1;
    int normal_processes_count = 0;

    for (pid_t pid : child_pids) {
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            SPR_LOGD("Parent process: Child process %d exited normally with code %d", pid, WEXITSTATUS(status));
            normal_processes_count++;
        }
        else if (WIFSIGNALED(status)) {
            SPR_LOGD("Parent process: Child process %d exited abnormally with signal %d", pid, WTERMSIG(status));
            crash_process_exit_status = WTERMSIG(status);
        }
    }

    // Expected results: Crash process should complete 2 accesses, others complete 3
    EXPECT_EQ(shared_data->access_count[CRASH_PROCESS_ID - 1], 2)
        << "Crash process should complete 2 accesses";

    for (int i = 0; i < NUM_PROCESSES; ++i) {
        if (i + 1 != CRASH_PROCESS_ID) {
            EXPECT_EQ(shared_data->access_count[i], MAX_ITERATIONS)
                << "Normal process should complete " << MAX_ITERATIONS << " accesses";
        }
    }

    // Verify crash process actually exited abnormally
    EXPECT_NE(crash_process_exit_status, -1) << "Crash process should exit abnormally";
    // Verify other processes exited normally
    EXPECT_EQ(normal_processes_count, NUM_PROCESSES - 1) << "Other processes should exit normally";

    // Clean up shared memory
    munmap(shared_data, sizeof(SharedData));
    close(shm_fd);
    shm_unlink("/test_crash_recovery_shm");
    shm_unlink(DEMO_SHARED_MUTEX.c_str()); // Clean up mutex
}
