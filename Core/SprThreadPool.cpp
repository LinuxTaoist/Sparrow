/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprThreadPool.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Singleton thread pool implementation (non-template logic)
 *                Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/12/11
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/12/11 | 1.0.0.1   | Xiang.D        | Align brace style, remove all exceptions
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "SprLog.h"
#include "SprThreadPool.h"

#define LOG_TAG "SprThrPool"

static std::atomic<bool> gObjAlive(true);

SprThreadPool* SprThreadPool::GetInstance(int32_t initWorkerCount)
{
    if (!gObjAlive) {
        return nullptr;
    }

    static SprThreadPool instance(initWorkerCount);
    return &instance;
}

SprThreadPool::SprThreadPool(int32_t initWorkerCount)
    : mInitWorkerCount(initWorkerCount), mIsPoolRunning(true), mTaskCntPeak(0), mIdleWorkerCount(0)
{
    AddWorkerThreads(initWorkerCount);
    SPR_LOGI("Init workers = %d, Max workers = %d", initWorkerCount, SPR_THREAD_POOL_MAX_WORKERS);
}

SprThreadPool::~SprThreadPool()
{
    gObjAlive = false;
    mIsPoolRunning = false;
    mTaskCond.notify_all();

    for (std::thread& worker : mWorkers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    SPR_LOGI("Destroy workers = %zu, Remaining tasks = %zu", mWorkers.size(), mTaskQueue.size());
}

int32_t SprThreadPool::GetIdleWorkerCount() const
{
    return mIdleWorkerCount;
}

int32_t SprThreadPool::GetTotalWorkerCount() const
{
    return static_cast<int32_t>(mWorkers.size());
}

int32_t SprThreadPool::DumpDetails() const
{
    SPR_LOGI("                           Dump Thread Poll Details                                            \n");
    SPR_LOGI("-----------------------------------------------------------------------------------------------\n");
    SPR_LOGI("- Init workers = %d \n", mInitWorkerCount);
    SPR_LOGI("- Max workers  = %d \n", SPR_THREAD_POOL_MAX_WORKERS);
    SPR_LOGI("- Idle workers = %d \n", mIdleWorkerCount.load());
    SPR_LOGI("- Total workers = %d \n", mWorkers.size());
    SPR_LOGI("- Task count peak = %d \n", mTaskCntPeak.load());
    SPR_LOGI("-----------------------------------------------------------------------------------------------\n");
    return 0;
}

void SprThreadPool::AddWorkerThreads(int32_t workerCount)
{
    const size_t currWorkerCount = mWorkers.size();

    // Add workers only if not exceeding max limit
    for (int32_t i = 0; i < workerCount; ++i) {
        if (currWorkerCount + i >= SPR_THREAD_POOL_MAX_WORKERS) {
            SPR_LOGE("Add workers failed! Reach max limit (%d)", SPR_THREAD_POOL_MAX_WORKERS);
            break;
        }

        // Create worker thread and bind to core loop logic
        // Increment idle count after worker creation
        mWorkers.emplace_back(&SprThreadPool::RunWorkerLoop, this);
        mIdleWorkerCount++;
    }
}

void SprThreadPool::RunWorkerLoop()
{
    // Worker loop: Run until pool stops and no pending tasks
    while (mIsPoolRunning) {
        Task currentTask;
        bool hasPendingTask = false;

        // Step 1: Get pending task from queue (thread-safe)
        {
            std::unique_lock<std::mutex> queueLock(mTaskQueueLock);
            // Wait for task or stop signal (avoid busy waiting)
            mTaskCond.wait(queueLock, [this]() {
                return !mIsPoolRunning || !mTaskQueue.empty();
            });

            // Exit loop if pool stops and no tasks left
            if (!mIsPoolRunning && mTaskQueue.empty()) {
                break;
            }

            // Take task from queue (FIFO order) and mark worker as busy
            if (!mTaskQueue.empty()) {
                currentTask = std::move(mTaskQueue.front());
                mTaskQueue.pop();
                mIdleWorkerCount--;
                hasPendingTask = true;
            }
        }

        // Step 2: Execute task (unlock first to reduce mutex holding time)
        if (hasPendingTask) {
            currentTask();
            // Mark worker as idle after task completion
            mIdleWorkerCount++;
        }
    }

    // Decrement idle count when worker exits
    mIdleWorkerCount--;
    SPR_LOGI("Worker exited! Remaining idle workers=%d", mIdleWorkerCount.load());
}
