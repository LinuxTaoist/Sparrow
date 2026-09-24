/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprObserver.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SprObserver 观察者基类内部测试
 *  @date       : 2026/09/10
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "SprMsg.h"
#include "SprSigId.h"
#include "CoreTypeDefs.h"
#include "SprObserver.h"
#include "gtest/gtest.h"

using namespace InternalDefs;

namespace {

// 测试用观察者派生类，仅实现纯虚 Init，并暴露 protected 方法
class TestObserver : public SprObserver {
public:
    explicit TestObserver(ModuleIDType id)
        : SprObserver(id, "TestObserver", MEDIATOR_PROXY_MQUEUE) {}

    int32_t SetLogLevel(int32_t level) { return SetPrintLogLevel(level); }
    int32_t SetLogLength(int32_t length) { return SetPrintLogLength(length); }

private:
    int32_t Init() override { return 0; }
};

const ModuleIDType kTestModuleId = static_cast<ModuleIDType>(MODULE_PUBLIC_END + 2000);
}

// 测试构造与模块 ID/名称访问
TEST(Core_SprObserver, ConstructorAndAccessors) {
    TestObserver obs(kTestModuleId);
    EXPECT_EQ(obs.GetModuleId(), kTestModuleId);
    EXPECT_EQ(obs.GetModuleName(), "TestObserver");
}

// 测试设置非法日志等级返回失败
TEST(Core_SprObserver, SetPrintLogLevelInvalid) {
    TestObserver obs(kTestModuleId);
    EXPECT_EQ(obs.SetLogLevel(-1), -1);
    EXPECT_EQ(obs.SetLogLevel(LOG_LEVEL_BUTT + 1), -1);
}

// 测试设置合法日志等级成功
TEST(Core_SprObserver, SetPrintLogLevelValid) {
    TestObserver obs(kTestModuleId);
    EXPECT_EQ(obs.SetLogLevel(LOG_LEVEL_DEBUG), 0);
    EXPECT_EQ(obs.SetLogLevel(LOG_LEVEL_INFO), 0);
}

// 测试设置非法日志长度返回失败
TEST(Core_SprObserver, SetPrintLogLengthInvalid) {
    TestObserver obs(kTestModuleId);
    EXPECT_EQ(obs.SetLogLength(-1), -1);
}

// 测试设置合法日志长度成功
TEST(Core_SprObserver, SetPrintLogLengthValid) {
    TestObserver obs(kTestModuleId);
    EXPECT_EQ(obs.SetLogLength(512), 0);
}

// 测试首次通知时懒获取 MediatorProxy
TEST(Core_SprObserver, NotifyObserverWithLazyProxy) {
    TestObserver obs(kTestModuleId);
    SprMsg msg(0x1234);
    EXPECT_EQ(obs.NotifyObserver(msg), 0);
}

// 测试首次按 ID 通知时懒获取 MediatorProxy
TEST(Core_SprObserver, NotifyObserverByIdWithLazyProxy) {
    TestObserver obs(kTestModuleId);
    SprMsg msg(0x1234);
    EXPECT_EQ(obs.NotifyObserver(kTestModuleId, msg), 0);
}

// 测试首次广播通知时懒获取 MediatorProxy
TEST(Core_SprObserver, NotifyAllObserverWithLazyProxy) {
    TestObserver obs(kTestModuleId);
    SprMsg msg(0x1234);
    EXPECT_EQ(obs.NotifyAllObserver(msg), 0);
}

// 测试首次注册/注销定时器时懒获取 MediatorProxy
TEST(Core_SprObserver, TimerWithLazyProxy) {
    TestObserver obs(kTestModuleId);
    EXPECT_EQ(obs.RegisterTimer(100, 100, 0x1234), 0);
    EXPECT_EQ(obs.UnregisterTimer(0x1234), 0);
}
