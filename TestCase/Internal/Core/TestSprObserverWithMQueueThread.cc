/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprObserverWithMQueueThread.cc
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
#include <thread>
#include <chrono>
#include <atomic>
#include "SprMsg.h"
#include "SprLog.h"
#include "gtest/gtest.h"
#include "SprObserverWithMQueueThread.h"

using namespace InternalDefs;

#define LOG_TAG "ObsThdTest"

// 测试组件：记录收到消息的计数、msgId 与处理线程 id
class TestObserverMQThread : public SprObserverWithMQueueThread
{
public:
    TestObserverMQThread()
        : SprObserverWithMQueueThread(InternalDefs::MODULE_GTEST_INTERNAL, "TestObserverMQThread"),
          mRecvCount(0),
          mLastMsgId(0),
          mProcessThreadId(std::thread::id()),
          mSlowMs(0)
    {
    }

    int32_t GetRecvCount() const { return mRecvCount.load(); }
    uint32_t GetLastMsgId() const { return mLastMsgId.load(); }
    std::thread::id GetProcessThreadId() const { return mProcessThreadId; }

    // 开启慢处理：ProcessMsg 模拟耗时，验证线程隔离
    void SetSlowProcessingMs(int32_t slowMs) { mSlowMs = slowMs; }

    // 模拟 epoll 回调：MQ 取消息 → ProcessRecvMsg 入线程队列
    int32_t TriggerRecv()
    {
        SprMsg msg;
        if (RecvMsg(msg) < 0) {
            SPR_LOGE("RecvMsg failed!\n");
            return -1;
        }

        return ProcessRecvMsg(msg);
    }

    // 直接向线程队列注入一条消息（验证 FIFO 与计数）
    int32_t InjectMsg(uint32_t msgId)
    {
        SprMsg msg(msgId);
        return ProcessRecvMsg(msg);
    }

private:
    int32_t Init() override { return 0; }
    int32_t ProcessMsg(const SprMsg& msg) override
    {
        mRecvCount++;
        mLastMsgId = msg.GetMsgId();
        mProcessThreadId = std::this_thread::get_id();

        int32_t slowMs = mSlowMs.load();
        if (slowMs > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(slowMs));
        }

        return 0;
    }

private:
    std::atomic<int32_t>  mRecvCount;
    std::atomic<uint32_t> mLastMsgId;
    std::thread::id       mProcessThreadId;
    std::atomic<int32_t>  mSlowMs;
};

// 工具：等待条件满足（带超时）
template <typename ConditionFunc>
static bool WaitUntil(ConditionFunc condition, int32_t timeoutMs)
{
    auto start = std::chrono::steady_clock::now();
    while (!condition()) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start).count() > timeoutMs) {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return true;
}

// 测试：ProcessRecvMsg 注入消息后，由独立线程异步处理（计数最终为 1）
TEST(Core_SprObserverWithMQueueThread, InjectedMsgProcessedByDedicatedThread)
{
    TestObserverMQThread observer;
    std::thread::id mainThreadId = std::this_thread::get_id();
    const uint32_t testMsgId = 0x11110000U;

    ASSERT_EQ(observer.InjectMsg(testMsgId), 0) << "InjectMsg failed";
    bool processed = WaitUntil([&]() { return observer.GetRecvCount() >= 1; }, 2000);
    ASSERT_TRUE(processed) << "Message not processed within timeout";

    EXPECT_EQ(observer.GetLastMsgId(), testMsgId) << "MsgId mismatch";
    // 关键：处理线程必须是组件专属线程，而非调用线程
    EXPECT_NE(observer.GetProcessThreadId(), mainThreadId)
        << "Message should be processed by the dedicated thread, not the caller";
}

// 测试：FIFO 保序 —— 连续注入多条消息，处理顺序与注入顺序一致
TEST(Core_SprObserverWithMQueueThread, MessagesProcessedInFifoOrder)
{
    TestObserverMQThread observer;
    const uint32_t msgIds[] = {0xAA000001U, 0xAA000002U, 0xAA000003U, 0xAA000004U, 0xAA000005U};

    for (uint32_t id : msgIds) {
        ASSERT_EQ(observer.InjectMsg(id), 0) << "InjectMsg failed for id " << id;
    }

    bool done = WaitUntil([&]() { return observer.GetRecvCount() >= 5; }, 3000);
    ASSERT_TRUE(done) << "Not all messages processed within timeout";
    EXPECT_EQ(observer.GetLastMsgId(), msgIds[4])
        << "Last processed msgId should be the last injected one (FIFO)";
}

// 测试：线程隔离 —— 慢处理（200ms）不阻塞消息入队，多次注入立即可返回
TEST(Core_SprObserverWithMQueueThread, SlowProcessingDoesNotBlockEnqueue)
{
    TestObserverMQThread observer;
    observer.SetSlowProcessingMs(200);   // 每条消息处理 200ms

    // 注入 3 条消息：若同步处理则 InjectMsg 会阻塞 200ms×3；线程模式应立即返回
    auto start = std::chrono::steady_clock::now();
    for (int32_t i = 0; i < 3; i++) {
        ASSERT_EQ(observer.InjectMsg(0xBB000000 + i), 0) << "InjectMsg failed";
    }
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();

    // 入队耗时远小于 3×200ms=600ms（允许调度噪声，阈值取 300ms）
    EXPECT_LT(elapsedMs, 300) << "Enqueue should not wait for slow processing, elapsed = " << elapsedMs << "ms";

    // 等待全部处理完，验证独立线程真正执行了慢任务
    bool done = WaitUntil([&]() { return observer.GetRecvCount() >= 3; }, 3000);
    ASSERT_TRUE(done) << "Slow messages not fully processed within timeout";
}

// 测试：SendMsg → MQ → TriggerRecv → 线程消费 全链路
TEST(Core_SprObserverWithMQueueThread, SendMsgRecvMsgThreadRoundTrip)
{
    TestObserverMQThread observer;
    const uint32_t testMsgId = 0xCC00DEAD;

    SprMsg sendMsg(testMsgId);
    sendMsg.SetU32Value(0xDEADBEEF);
    ASSERT_EQ(observer.SendMsg(sendMsg), 0) << "SendMsg failed";

    ASSERT_EQ(observer.TriggerRecv(), 0) << "TriggerRecv failed";
    bool processed = WaitUntil([&]() { return observer.GetRecvCount() >= 1; }, 2000);
    ASSERT_TRUE(processed) << "Message not processed within timeout";

    EXPECT_EQ(observer.GetLastMsgId(), testMsgId) << "MsgId mismatch after MQ round-trip";
}
