/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestModuleThread.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/08/18
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/08/18 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __TEST_MODULE_THREAD_H__
#define __TEST_MODULE_THREAD_H__

#include <mutex>
#include <atomic>
#include <condition_variable>
#include "SprObserverWithMQueueThread.h"

class TestModuleThread : public SprObserverWithMQueueThread
{
public:
    TestModuleThread();
    virtual ~TestModuleThread();

    void ResetRecvCount() { mRecvCount = 0; }
    int32_t GetRecvCount() const { return mRecvCount.load(); }

    // 慢处理开关：ProcessMsg 模拟耗时任务，用于验证线程隔离
    void SetSlowProcessingMs(int32_t slowMs) { mSlowProcessingMs = slowMs; }

    // 让 ProcessMsg 记录"最后一次处理线程 id"，用于验证消息确实在独立线程处理
    std::thread::id GetLastProcessThreadId() const { return mLastProcessThreadId; }

    // 模拟 epoll 回调：从 MQ 收一条消息并送入线程处理队列（供测试直接驱动）
    int32_t TriggerRecv();

private:
    int32_t Init() override;
    int32_t ProcessMsg(const SprMsg& msg) override;

private:
    std::atomic<int32_t>  mRecvCount;           // 收到的业务消息计数
    std::atomic<int32_t>  mSlowProcessingMs;    // 模拟慢处理的毫秒数，0=不模拟
    std::thread::id       mLastProcessThreadId; // 最后一次 ProcessMsg 的线程 id
};

#endif // __TEST_MODULE_THREAD_H__
