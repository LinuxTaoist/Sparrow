/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprObserverWithTimerfd.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SprObserverWithTimerfd 基于 timerfd 的观察者内部测试
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
#include <atomic>
#include "SprSigId.h"
#include "CoreTypeDefs.h"
#include "SprObserverWithTimerfd.h"
#include "gtest/gtest.h"

using namespace InternalDefs;

namespace {

// 测试用派生类，实现纯虚 ProcessTimerEvent
class TestTimerfdObserver : public SprObserverWithTimerfd {
public:
    explicit TestTimerfdObserver(ModuleIDType id)
        : SprObserverWithTimerfd(id, "TestTimerfdObserver", MEDIATOR_PROXY_MQUEUE),
          mTimerCount(0) {}

    int32_t GetTimerCount() const { return mTimerCount.load(); }

private:
    int32_t Init() override { return 0; }

    int32_t ProcessTimerEvent() override {
        mTimerCount.fetch_add(1);
        return 0;
    }

    std::atomic<int32_t> mTimerCount;
};

const ModuleIDType kTestModuleId = static_cast<ModuleIDType>(MODULE_PUBLIC_END + 3000);
}

// 测试构造
TEST(Core_SprObserverWithTimerfd, Constructor)
{
    TestTimerfdObserver obs(kTestModuleId);
    EXPECT_EQ(obs.GetModuleId(), kTestModuleId);
    EXPECT_EQ(obs.GetModuleName(), "TestTimerfdObserver");
}

// 测试框架初始化（注册到 epoll）
TEST(Core_SprObserverWithTimerfd, InitFramework)
{
    TestTimerfdObserver obs(kTestModuleId);
    // InitFramework 调用 AddToPoll，返回 0 且不崩溃
    obs.Initialize();
}

// 测试 EpollEvent 传入错误 fd 返回空
TEST(Core_SprObserverWithTimerfd, EpollEventInvalidFd)
{
    TestTimerfdObserver obs(kTestModuleId);
    EXPECT_EQ(obs.EpollEvent(-999, EPOLL_TYPE_TIMERFD, nullptr), nullptr);
}

// 测试 timerfd 定时事件触发 ProcessTimerEvent
TEST(Core_SprObserverWithTimerfd, TimerEventTriggersProcess)
{
    TestTimerfdObserver obs(kTestModuleId);
    // PTimer 构造已 InitTimer，直接启动定时器即可
    ASSERT_GE(obs.GetEvtFd(), 0);
    ASSERT_EQ(obs.StartTimer(10), 0);   // 10ms 后触发一次

    usleep(50 * 1000);

    // 事件到达后调用 EpollEvent 触发 ProcessTimerEvent
    obs.EpollEvent(obs.GetEvtFd(), EPOLL_TYPE_TIMERFD, nullptr);
    EXPECT_GE(obs.GetTimerCount(), 1);
}
