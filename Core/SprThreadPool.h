/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprThreadPool.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Singleton thread pool implementation (C++11 compatible)
 *                Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/09/14
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/12/11 | 1.0.0.1   | Xiang.D        |
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_THREAD_POOL_H__
#define __SPR_THREAD_POOL_H__

#include <vector>
#include <queue>
#include <atomic>
#include <future>
#include <mutex>
#include <thread>
#include <functional>
#include <memory>

// Thread pool config: Max number of worker threads (keep moderate for efficiency)
#define SPR_THREAD_POOL_MAX_WORKERS 16

class SprThreadPool
{
public:
    /**
     * @brief Get singleton instance of thread pool
     * @param initWorkerCount Initial number of worker threads (only effective on first call, default: 4)
     * @return Singleton pointer of SprThreadPool
     */
    static SprThreadPool* GetInstance(int32_t initWorkerCount = 4);

    SprThreadPool(const SprThreadPool&) = delete;
    SprThreadPool& operator=(const SprThreadPool&) = delete;
    SprThreadPool(SprThreadPool&&) = delete;
    SprThreadPool& operator=(SprThreadPool&&) = delete;

    /**
     * @brief Get count of idle worker threads
     * @return Idle worker count (atomic read)
     */
    int32_t GetIdleWorkerCount() const;

    /**
     * @brief Get total count of worker threads in pool
     * @return Total worker thread count
     */
    int32_t GetTotalWorkerCount() const;

    /**
     * @brief Submit task with return value (support variable parameters)
     * @tparam F Task function type (global/static member/lambda)
     * @tparam Args Task parameter types
     * @param taskFunc Task function to execute
     * @param args Variable parameters for the task function
     * @return Future object for task result (invalid if pool is stopped)
     */
    template<class F, class... Args>
    auto SubmitTask(F&& taskFunc, Args&&... args) -> std::future<decltype(taskFunc(args...))>;

    /**
     * @brief Submit task without return value (simplified version)
     * @tparam F Task function type (void return, any parameters)
     * @param task Task function/lambda to execute
     */
    template<class F>
    void SubmitTask(F&& task);

private:
    /**
     * @brief Private constructor (singleton pattern)
     * @param initWorkerCount Initial number of worker threads
     */
    explicit SprThreadPool(int32_t initWorkerCount);

    /**
     * @brief Private destructor (stop pool and clean resources)
     */
    ~SprThreadPool();

    /**
     * @brief Add specified number of worker threads to the pool
     * @param workerCount Number of worker threads to add
     */
    void AddWorkerThreads(int32_t workerCount);

    /**
     * @brief Core loop logic for each worker thread
     */
    void RunWorkerLoop();

private:
    int32_t mInitWorkerCount;  // Initial worker thread count (fixed after init)
    using Task = std::function<void()>;     // Generic task type definition
    std::atomic<bool> mIsPoolRunning;       // Flag indicating if pool is running
    std::atomic<int32_t> mIdleWorkerCount;      // Count of currently idle workers
    std::vector<std::thread> mWorkers;      // Container for worker threads
    std::queue<Task> mTaskQueue;            // FIFO queue for pending tasks
    std::mutex mTaskQueueLock;              // Mutex for synchronizing access to task queue
    std::condition_variable mTaskCond;      // Condition variable to wake idle workers
};

template<class F, class... Args>
auto SprThreadPool::SubmitTask(F&& taskFunc, Args&&... args) -> std::future<decltype(taskFunc(args...))>
{
    // Return invalid future if pool has stopped (no exception throw)
    if (!mIsPoolRunning) {
        // printf("[SprThreadPool] Submit failed: Pool is stopped\n");
        return std::future<decltype(taskFunc(args...))>();
    }

    // Deduce task return type and wrap task with packaged_task
    using RetType = decltype(taskFunc(args...));
    auto taskWrapper = std::make_shared<std::packaged_task<RetType()>>(
        std::bind(std::forward<F>(taskFunc), std::forward<Args>(args)...)
    );

    // Add wrapped task to queue (with thread-safe protection)
    {
        std::lock_guard<std::mutex> lock(mTaskQueueLock);
        mTaskQueue.emplace([taskWrapper]() {
            // Catch internal exceptions to prevent worker crash (no rethrow)
            (*taskWrapper)();
        });
    }

    // Wake up one idle worker to process the new task
    mTaskCond.notify_one();
    return taskWrapper->get_future();
}

template<class F>
void SprThreadPool::SubmitTask(F&& task)
{
    // Do nothing if pool has stopped (no exception throw)
    if (!mIsPoolRunning) {
        // printf("[SprThreadPool] Submit failed: Pool is stopped\n");
        return;
    }

    // Add task to queue (thread-safe)
    {
        std::lock_guard<std::mutex> lock(mTaskQueueLock);
        mTaskQueue.emplace(std::forward<F>(task));
    }

    // Wake up one idle worker
    mTaskCond.notify_one();
}

#endif // __SPR_THREAD_POOL_H__
