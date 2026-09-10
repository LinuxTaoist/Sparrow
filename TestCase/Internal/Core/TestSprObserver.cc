/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprObserver.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SprObserver 观察者基类内部测试
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
TEST(Core_SprObserver, ConstructorAndAccessors)
{
    TestObserver obs(kTestModuleId);
    EXPECT_EQ(obs.GetModuleId(), kTestModuleId);
    EXPECT_EQ(obs.GetModuleName(), "TestObserver");
}

// 测试设置非法日志等级返回失败
TEST(Core_SprObserver, SetPrintLogLevelInvalid)
{
    TestObserver obs(kTestModuleId);
    EXPECT_EQ(obs.SetLogLevel(-1), -1);
    EXPECT_EQ(obs.SetLogLevel(LOG_LEVEL_BUTT + 1), -1);
}

// 测试设置合法日志等级成功
TEST(Core_SprObserver, SetPrintLogLevelValid)
{
    TestObserver obs(kTestModuleId);
    EXPECT_EQ(obs.SetLogLevel(LOG_LEVEL_DEBUG), 0);
    EXPECT_EQ(obs.SetLogLevel(LOG_LEVEL_INFO), 0);
}

// 测试设置非法日志长度返回失败
TEST(Core_SprObserver, SetPrintLogLengthInvalid)
{
    TestObserver obs(kTestModuleId);
    EXPECT_EQ(obs.SetLogLength(-1), -1);
}

// 测试设置合法日志长度成功
TEST(Core_SprObserver, SetPrintLogLengthValid)
{
    TestObserver obs(kTestModuleId);
    EXPECT_EQ(obs.SetLogLength(512), 0);
}

// 测试未初始化时通知观察者返回失败
TEST(Core_SprObserver, NotifyObserverWithoutProxy)
{
    TestObserver obs(kTestModuleId);
    SprMsg msg(0x1234);
    EXPECT_EQ(obs.NotifyObserver(msg), -1);
}

// 测试未初始化时通知指定 ID 观察者返回失败
TEST(Core_SprObserver, NotifyObserverByIdWithoutProxy)
{
    TestObserver obs(kTestModuleId);
    SprMsg msg(0x1234);
    EXPECT_EQ(obs.NotifyObserver(kTestModuleId, msg), -1);
}

// 测试未初始化时通知所有观察者返回失败
TEST(Core_SprObserver, NotifyAllObserverWithoutProxy)
{
    TestObserver obs(kTestModuleId);
    SprMsg msg(0x1234);
    EXPECT_EQ(obs.NotifyAllObserver(msg), -1);
}

// 测试未初始化时注册/注销定时器返回失败
TEST(Core_SprObserver, TimerWithoutProxy)
{
    TestObserver obs(kTestModuleId);
    EXPECT_EQ(obs.RegisterTimer(100, 100, 0x1234), -1);
    EXPECT_EQ(obs.UnregisterTimer(0x1234), -1);
}
