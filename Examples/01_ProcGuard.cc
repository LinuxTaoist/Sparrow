/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DebugBinder.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/03/31
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/31 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <thread>
#include "ProcMutex.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

const std::string DEMO_SHARED_MUTEX = "demo_shared_mutex";

void childProcessWork(int processId, bool isCrashProcess)
{
    std::mutex threadMutex;
    ProcMutex procMutex(DEMO_SHARED_MUTEX);
    SPR_LOG("进程 %d: 启动，准备竞争锁...\n", processId);

    // 循环尝试访问共享资源
    for (int i = 0; i < 3; ++i) {
        ProcLockGuard lockGuard(procMutex, threadMutex);
        SPR_LOG("进程 %d: 成功获取锁，正在访问资源（第%d次）\n", processId, i+1);
        sleep(1);

        // 特定进程在持有锁时异常退出（模拟崩溃）
        if (isCrashProcess && i == 1) {
            SPR_LOG("进程 %d: 即将异常退出（持有锁状态）！\n", processId);
            std::abort();
        }

        SPR_LOG("进程 %d: 释放锁，等待下一次竞争...\n", processId);
    }

    SPR_LOG("进程 %d: 正常退出\n", processId);
}

int main()
{
    const int NUM_PROCESSES     = 3;  // 总进程数
    const int CRASH_PROCESS_ID  = 1;  // 指定崩溃的进程ID

    SPR_LOG("=== 跨进程互斥锁演示程序启动 ===       \n"
            "演示内容:                              \n"
            "1. %d个进程竞争同一个共享资源          \n"
            "2. 进程 %d: 在持有锁时异常退出          \n"
            "3. 验证其他进程能否检测并恢复锁状态    \n\n", NUM_PROCESSES, CRASH_PROCESS_ID);

    srand(time(nullptr));
    std::vector<pid_t> childPids;

    // 创建多个子进程
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            SPR_LOG("fork失败: %s\n", strerror(errno));
            return EXIT_FAILURE;
        } else if (pid == 0) {
            // 子进程逻辑：第 CRASH_PROCESS_ID 个进程会崩溃
            childProcessWork(i + 1, (i + 1 == CRASH_PROCESS_ID));
            return EXIT_SUCCESS;
        } else {
            childPids.push_back(pid);
            usleep(50000);  // 错开进程启动时间
        }
    }

    // 父进程等待所有子进程结束
    int status;
    for (pid_t pid : childPids) {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            SPR_LOG("父进程：子进程 %d: 正常退出，退出码 %d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            SPR_LOG("父进程：子进程 %d: 异常退出，信号 %d\n", pid, WTERMSIG(status));
        }
    }

    SPR_LOG("\n=== 演示结束 ===\n");
    return EXIT_SUCCESS;
}
