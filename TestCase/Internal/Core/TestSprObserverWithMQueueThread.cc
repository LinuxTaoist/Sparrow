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
 */
#include <string>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
#include <memory>
#include <functional>
#include "SprMsg.h"
#include "SprLog.h"
#include "gtest/gtest.h"
#include "SprEpollSchedule.h"
#include "SprObserverWithMQueueThread.h"

using namespace InternalDefs;

#define LOG_TAG "ObsThdTest"
#define TEST_WAIT_TIMEOUT_MS 2000

const ModuleIDType TEST_MODULE_ID_A = static_cast<ModuleIDType>(MODULE_PUBLIC_END + 1000);
const ModuleIDType TEST_MODULE_ID_B = static_cast<ModuleIDType>(MODULE_PUBLIC_END + 1001);

// 测试组件：线程模式组件
class TestObsThread : public SprObserverWithMQueueThread {
public:
    explicit TestObsThread(ModuleIDType id)
        : SprObserverWithMQueueThread(id, "TestObsThread"),
          mProcessThreadId(std::thread::id()),
          mIsRcved(false),
          mRecvCount(0),
          mSlowMs(0) {}

    bool  IsRcved() const {
        return mIsRcved.load();
    }

    int32_t GetRecvCount() const {
        return mRecvCount.load();
    }

    void  setSlowMs(int32_t slowMs) {
        mSlowMs.store(slowMs);
    }

    SprMsg GetMsg() const {
        std::lock_guard<std::mutex> lock(mRecvMutex);
        return mMsg;
    }

    std::thread::id GetProcessThreadId() const {
        std::lock_guard<std::mutex> lock(mRecvMutex);
        return mProcessThreadId;
    }

    std::vector<uint32_t> GetRecvMsgIds() const {
        std::lock_guard<std::mutex> lock(mRecvMutex);
        return mRecvMsgIds;
    }

private:
    int32_t Init() override {
        return 0;
    }

    int32_t ProcessMsg(const SprMsg& msg) override {
        SPR_LOGD("ProcessMsg: msgId: 0x%x", msg.GetMsgId());
        {
            std::lock_guard<std::mutex> lock(mRecvMutex);
            mMsg = msg;
            mRecvMsgIds.push_back(msg.GetMsgId());
            mProcessThreadId = std::this_thread::get_id();
        }
        mRecvCount.fetch_add(1);
        mIsRcved.store(true);

        if (mSlowMs.load() > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(mSlowMs.load()));
        }

        return 0;
    }

private:
    SprMsg                mMsg;
    mutable std::mutex    mRecvMutex;
    std::vector<uint32_t> mRecvMsgIds;
    std::thread::id       mProcessThreadId;
    std::atomic<bool>     mIsRcved;
    std::atomic<int32_t>  mRecvCount;
    std::atomic<int32_t>  mSlowMs;
};

class Core_SprObserverWithMQueueThread : public ::testing::Test {
protected:
    static void SetUpTestCase()
    {
        SPR_LOGD("SetUpTestCase enter!");
        mCaseIndex = 0;
        mThread = std::thread([&]() {
            SprEpollSchedule::GetInstance(0, 2000)->EpollLoop();
        });

        SPR_LOGD("SetUpTestCase exit!");
    }

    static void TearDownTestCase()
    {
        SPR_LOGD("TearDownTestCase enter!");
        SprEpollSchedule::GetInstance(0, 2000)->ExitLoop();
        mThread.join();
        SPR_LOGD("TearDownTestCase exit!");
    }

public:
    static int32_t mCaseIndex;
    static std::thread mThread;
};

int32_t Core_SprObserverWithMQueueThread::mCaseIndex = 0;
std::thread Core_SprObserverWithMQueueThread::mThread;

static bool waitUntil(std::function<bool()> condition, int32_t timeoutMs) {
    const auto startTime = std::chrono::steady_clock::now();
    while (!condition()) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count() > timeoutMs) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return condition();
}

// 测试：SendMsg 自收发，消息经 MQ 由独立线程处理，payload 保持一致
TEST_F(Core_SprObserverWithMQueueThread, SendMsgSelfRoundTrip) {
    SPR_LOGD("[  Case %02d  ] SendMsgSelfRoundTrip", ++mCaseIndex);
    TestObsThread observer(TEST_MODULE_ID_A);
    observer.Initialize();

    std::thread::id mainThreadId = std::this_thread::get_id();
    const uint32_t msgId = 0xCAFE0001U;
    const std::string payload = "hello thread observer";

    SprMsg sendMsg(msgId);
    sendMsg.SetString(payload);

    EXPECT_TRUE(waitUntil([&]() { return observer.IsConnected(); }, 200));
    EXPECT_EQ(0, observer.SendMsg(sendMsg));
    EXPECT_TRUE(waitUntil([&]() { return observer.IsRcved(); }, TEST_WAIT_TIMEOUT_MS));

    EXPECT_NE(mainThreadId, observer.GetProcessThreadId());
    EXPECT_EQ(sendMsg.GetMsgId(), observer.GetMsg().GetMsgId());
    EXPECT_EQ(sendMsg.GetString(), observer.GetMsg().GetString());
}

// 测试：多字段 payload 经 MQ 往返后数据保持一致
TEST_F(Core_SprObserverWithMQueueThread, MultiFieldPayloadAccuracy) {
    SPR_LOGD("[  Case %02d  ] MultiFieldPayloadAccuracy", ++mCaseIndex);
    TestObsThread observer(TEST_MODULE_ID_A);
    observer.Initialize();

    const uint32_t msgId = 0xCAFE0021U;
    const std::string payload = "accuracy-check-0123456789";

    SprMsg sendMsg(msgId);
    sendMsg.SetString(payload);
    sendMsg.SetU32Value(0xDEADBEEFU);
    sendMsg.SetI64Value(-0x1122334455667788LL);
    EXPECT_EQ(0, observer.SendMsg(sendMsg)) << "SendMsg failed";

    EXPECT_TRUE(waitUntil([&]() { return observer.IsRcved(); }, TEST_WAIT_TIMEOUT_MS))
        << "Message not processed within timeout";

    EXPECT_EQ(sendMsg.GetMsgId(), observer.GetMsg().GetMsgId());
    EXPECT_EQ(sendMsg.GetString(), observer.GetMsg().GetString());
    EXPECT_EQ(sendMsg.GetU32Value(), observer.GetMsg().GetU32Value());
    EXPECT_EQ(sendMsg.GetI64Value(), observer.GetMsg().GetI64Value());
}

// 测试：连续注入多条消息，按 FIFO 顺序处理
TEST_F(Core_SprObserverWithMQueueThread, FifoOrderPreserved) {
    SPR_LOGD("[  Case %02d  ] FifoOrderPreserved", ++mCaseIndex);
    TestObsThread observer(TEST_MODULE_ID_A);
    observer.Initialize();

    const uint32_t msgIds[] = {0xCAF00001U, 0xCAF00002U, 0xCAF00003U, 0xCAF00004U, 0xCAF00005U};
    const int32_t msgCount = static_cast<int32_t>(sizeof(msgIds) / sizeof(msgIds[0]));

    for (int32_t i = 0; i < msgCount; i++) {
        observer.SendMsg(msgIds[i]);
    }

    EXPECT_TRUE(waitUntil([&]() { return observer.GetRecvCount() >= msgCount; }, TEST_WAIT_TIMEOUT_MS))
        << "Messages not processed within timeout";

    std::vector<uint32_t> recvIds = observer.GetRecvMsgIds();
    EXPECT_EQ(msgCount, static_cast<int32_t>(recvIds.size()));
    for (int32_t i = 0; i < msgCount && i < (int32_t)recvIds.size(); i++) {
        EXPECT_EQ(msgIds[i], recvIds[i]) << "FIFO order broken at index "
        << i << ": " << recvIds[i] << " != " << msgIds[i];
    }
}

// 测试：线程组件消息处理互不阻塞用例，A 慢处理不阻塞 B 的消息处理
TEST_F(Core_SprObserverWithMQueueThread, SlowProcessingDoesNotBlockEnqueue) {
    SPR_LOGD("[  Case %02d  ] SlowProcessingDoesNotBlockEnqueue", ++mCaseIndex);
    TestObsThread observerA(TEST_MODULE_ID_A);
    TestObsThread observerB(TEST_MODULE_ID_B);
    observerA.Initialize();
    observerB.Initialize();
    observerA.setSlowMs(2000);

    for (int32_t i = 0; i < 3; i++) {
        observerA.SendMsg(0xCAF00001U + i);
    }
    observerB.SendMsg(0xCAF00010U);

    EXPECT_TRUE(waitUntil([&]() { return observerB.IsRcved(); }, 1000))
        << "ObserverB blocked by slow ObserverA";
    EXPECT_EQ(0xCAF00010U, observerB.GetMsg().GetMsgId()) << "MsgId mismatch";
    EXPECT_LT(observerA.GetRecvCount(), 3) << "ObserverA should still be processing";
}

// 测试：模块互发，A 经 NotifyObserver 接口发消息给 B，B 独立线程处理
TEST_F(Core_SprObserverWithMQueueThread, ModuleMutualSendRecv) {
    SPR_LOGD("[  Case %02d  ] ModuleMutualSendRecv", ++mCaseIndex);
    TestObsThread observerA(TEST_MODULE_ID_A);
    TestObsThread observerB(TEST_MODULE_ID_B);
    observerA.Initialize();
    observerB.Initialize();

    const uint32_t msgId = 0xCAFE0001U;
    const std::string payload = "hello thread observer";

    SprMsg sendMsg(msgId);
    sendMsg.SetString(payload);
    observerA.NotifyObserver(observerB.GetModuleId(), sendMsg);

    EXPECT_TRUE(waitUntil([&]() { return observerB.IsRcved(); }, TEST_WAIT_TIMEOUT_MS))
        << "Message not processed within timeout";

    const SprMsg& recvMsg = observerB.GetMsg();
    EXPECT_EQ(sendMsg.GetMsgId(), recvMsg.GetMsgId()) << "MsgId mismatch";
    EXPECT_EQ(sendMsg.GetString(), recvMsg.GetString()) << "Payload mismatch";
}
