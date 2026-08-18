/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprObserverWithMQueue.cc
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
#include <string>
#include "SprMsg.h"
#include "SprLog.h"
#include "gtest/gtest.h"
#include "SprObserverWithMQueue.h"

using namespace InternalDefs;

#define LOG_TAG "ObsMQTest"

// 测试组件：实现纯虚 ProcessMsg，并记录收到的业务消息
class TestObserverMQ : public SprObserverWithMQueue
{
public:
    TestObserverMQ()
        : SprObserverWithMQueue(InternalDefs::MODULE_GTEST_INTERNAL, "TestObserverMQ"),
          mRecvCount(0),
          mLastMsgId(0)
    {
    }

    int32_t GetRecvCount() const { return mRecvCount; }
    uint32_t GetLastMsgId() const { return mLastMsgId; }

    // 公开包装：从 MQ 收一条消息并解码（RecvMsg 在基类为 protected）
    int32_t RecvOnce(SprMsg& msg) { return RecvMsg(msg); }

private:
    int32_t Init() override { return 0; }
    int32_t ProcessMsg(const SprMsg& msg) override
    {
        mRecvCount++;
        mLastMsgId = msg.GetMsgId();
        return 0;
    }

private:
    int32_t mRecvCount;
    uint32_t mLastMsgId;
};

// 测试：SendMsg 自收发链路，消息编码后经 MQ 回传，RecvMsg 可正确解码
TEST(Core_SprObserverWithMQueue, SendMsgThenRecvMsgRoundTrip)
{
    TestObserverMQ observer;
    const uint32_t testMsgId = 0x12345678;

    SprMsg sendMsg(testMsgId);
    sendMsg.SetI32Value(12345);
    ASSERT_EQ(observer.SendMsg(sendMsg), 0) << "SendMsg failed";

    SprMsg recvMsg;
    ASSERT_EQ(observer.RecvOnce(recvMsg), 0) << "RecvMsg failed";
    EXPECT_EQ(recvMsg.GetMsgId(), testMsgId) << "MsgId mismatch after round-trip";
    EXPECT_EQ(recvMsg.GetI32Value(), 12345) << "Payload mismatch after round-trip";
}

// 测试：默认 ProcessRecvMsg 行为 = 立即 DispatchSprMsg（同步处理）
TEST(Core_SprObserverWithMQueue, DefaultProcessRecvMsgDispatchesSynchronously)
{
    TestObserverMQ observer;
    const uint32_t testMsgId = 0x55667788U;
    SprMsg msg(testMsgId);
    ASSERT_EQ(observer.ProcessRecvMsg(msg), 0) << "ProcessRecvMsg failed";
    EXPECT_EQ(observer.GetRecvCount(), 1) << "Default ProcessRecvMsg should dispatch immediately";
    EXPECT_EQ(observer.GetLastMsgId(), testMsgId) << "Dispatched msgId mismatch";
}
