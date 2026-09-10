/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestAsyncEvent.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : AsyncEvent 异步事件通知内部测试
 *  @date       : 2026/09/10
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/10 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <string>
#include "AsyncEvent.h"
#include "gtest/gtest.h"

namespace {
std::string MakeEventName()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    char buf[96] = {};
    std::snprintf(buf, sizeof(buf), "spr_aevent_%d_%ld", getpid(), ts.tv_nsec % 1000000L);
    return std::string(buf);
}
}

class Util_AsyncEvent : public ::testing::Test {
protected:
    void SetUp() override {
        mName = MakeEventName();
    }

    void TearDown() override {
        unlink(("/tmp/" + mName + "_event").c_str());
    }

    std::string mName;
};

// 测试单例模式返回非空
TEST_F(Util_AsyncEvent, SingletonReturnsSameInstance)
{
    AsyncEvent* a = AsyncEvent::GetInstance();
    AsyncEvent* b = AsyncEvent::GetInstance();
    ASSERT_TRUE(a != nullptr);
    EXPECT_EQ(a, b);
}

// 测试未初始化 Parcel 时 EventNotify 返回失败
TEST_F(Util_AsyncEvent, EventNotifyWithoutParcel)
{
    AsyncEvent* event = AsyncEvent::GetInstance();
    EXPECT_EQ(event->EventNotify(1, nullptr, 0), -1);
}

// 测试作为写者初始化并发送事件
TEST_F(Util_AsyncEvent, AsWriterAndNotify)
{
    AsyncEvent* event = AsyncEvent::GetInstance();
    ASSERT_EQ(event->AsWriter(mName), 0);

    int32_t data = 12345;
    EXPECT_EQ(event->EventNotify(0x55, &data, sizeof(data)), 0);
    EXPECT_EQ(event->EventNotify(0x66, nullptr, 0), 0);
}

// 测试注册空回调返回失败
TEST_F(Util_AsyncEvent, RegisterNullCallbackFails)
{
    AsyncEvent* event = AsyncEvent::GetInstance();
    EXPECT_EQ(event->RegisterEventCallback(nullptr), -1);
}

// 测试取消注册事件回调
TEST_F(Util_AsyncEvent, UnregisterEventCallback)
{
    AsyncEvent* event = AsyncEvent::GetInstance();
    EXPECT_EQ(event->UnregisterEventCallback(), 0);
}

// 测试作为读者初始化
TEST_F(Util_AsyncEvent, AsReader)
{
    AsyncEvent* event = AsyncEvent::GetInstance();
    // 先以写者身份创建共享资源，再切换为读者
    ASSERT_EQ(event->AsWriter(mName), 0);
    ASSERT_EQ(event->AsReader(mName), 0);
}
