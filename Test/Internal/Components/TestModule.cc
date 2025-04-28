/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestModule.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/04/19
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/04/19 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <atomic>
#include "SprLog.h"
#include "TestModule.h"
#include "CoreTypeDefs.h"
#include "SprEpollSchedule.h"
#include "SprSigId.h"

using namespace InternalDefs;

#define LOG_TAG "TestModule"

TestModule::TestModule()
    : SprObserverWithMQueue(InternalDefs::MODULE_GTEST_INTERNAL, "TestInternal")
{
    mTimerCount = 0;
}

TestModule::~TestModule()
{
}

void TestModule::InitEnv()
{
    SPR_LOGD("TestModule InitEnv!");

    // 初始化组件框架
    Initialize();

    // 创建SprMs消息调度线程
    mpMsgThread = std::make_shared<std::thread>([]() {
        auto pSchedule = SprEpollSchedule::GetInstance();
        if (!pSchedule) {
            SPR_LOGE("Get Schedule Instance Failed!");
            return;
        }

        pSchedule->EpollLoop();
    });

    SPR_LOGD("TestModule InitEnv OK!");
}

void TestModule::DeInitEnv()
{
    SPR_LOGD("TestModule DeInitEnv!");

    auto pSchedule = SprEpollSchedule::GetInstance();
    if (!pSchedule) {
        SPR_LOGE("Get Schedule Instance Failed!");
        return;
    }

    pSchedule->ExitLoop();
    mpMsgThread->join();
    SPR_LOGD("TestModule DeInitEnv OK!");
}

int32_t TestModule::ResetTimerCnt()
{
    mTimerCount = 0;
    return 0;
}

int32_t TestModule::CondNotify()
{
    mCond.notify_one();
    return 0;
}

int32_t TestModule::CondWait(int32_t timeoutMs, int32_t value)
{
    std::unique_lock<std::mutex> lock(mMutex);
    mCond.wait_for(lock, std::chrono::milliseconds(timeoutMs), [&]() {
        SPR_LOGD("Waiting: cnt = %d, v = %d", mTimerCount, value);
        return (mTimerCount == value);
    });
    return 0;
}

int32_t TestModule::Init()
{
    return 0;
}

int32_t TestModule::ProcessMsg(const SprMsg& msg)
{
    switch (msg.GetMsgId()) {
        case SIG_ID_TEST_MODULE_200MS_TIMER_EVENT:
        case SIG_ID_TEST_MODULE_500MS_TIMER_EVENT:
        case SIG_ID_TEST_MODULE_1S_TIMER_EVENT:
        case SIG_ID_TEST_MODULE_2S_TIMER_EVENT:
        case SIG_ID_TEST_MODULE_3S_TIMER_EVENT:
        case SIG_ID_TEST_MODULE_5S_TIMER_EVENT: {
            mTimerCount++;
            mCond.notify_one();
            break;
        }
        default:
            break;
    }
    return 0;
}

std::shared_ptr<TestModule> TestSprComponents::mpTestModule = nullptr;

void TestSprComponents::SetUpTestCase()
{
    SPR_LOGD("TestModule SetUpTestCase!");
    mpTestModule = std::make_shared<TestModule>();
    mpTestModule->InitEnv();
}

void TestSprComponents::TearDownTestCase()
{
    SPR_LOGD("TestModule TearDownTestCase!");
    mpTestModule->DeInitEnv();
    mpTestModule = nullptr;
}
