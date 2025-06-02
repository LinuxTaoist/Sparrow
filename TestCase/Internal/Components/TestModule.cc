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
    m200MSCnt = 0;
    m500MSCnt = 0;
    m1SCnt = 0;
    m2SCnt = 0;
    m3SCnt = 0;
    m5SCnt = 0;
}

TestModule::~TestModule()
{
}

int32_t TestModule::CondNotify()
{
    mCond.notify_one();
    return 0;
}

int32_t TestModule::CondWait(int32_t timeoutMs)
{
    std::unique_lock<std::mutex> lock(mMutex);
    mCond.wait_for(lock, std::chrono::milliseconds(timeoutMs), []() {
        return false;
    });
    return 0;
}

int32_t TestModule::CondWait(int32_t timeoutMs, int32_t expectValue, const int32_t& actualValue)
{
    std::unique_lock<std::mutex> lock(mMutex);
    mCond.wait_for(lock, std::chrono::milliseconds(timeoutMs), [&]() {
        SPR_LOGD("Waiting: expect = %d, actual = %d", expectValue, actualValue);
        return (expectValue == actualValue);
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
        case SIG_ID_TEST_MODULE_200MS_TIMER_EVENT: {
            m200MSCnt++;
            break;
        }
        case SIG_ID_TEST_MODULE_500MS_TIMER_EVENT: {
            m500MSCnt++;
            break;
        }
        case SIG_ID_TEST_MODULE_1S_TIMER_EVENT: {
            m1SCnt++;
            break;
        }
        case SIG_ID_TEST_MODULE_2S_TIMER_EVENT: {
            m2SCnt++;
            break;
        }
        case SIG_ID_TEST_MODULE_3S_TIMER_EVENT: {
            m3SCnt++;
            break;
        }
        case SIG_ID_TEST_MODULE_5S_TIMER_EVENT: {
            m5SCnt++;
            break;
        }
        default:
            break;
    }

    mCond.notify_one();
    return 0;
}

int32_t TestSprComponents::mCaseIndex = 0;
EpollEventHandler* TestSprComponents::mpEpollSchedule = nullptr;
std::shared_ptr<std::thread> TestSprComponents::mpMsgThread = nullptr;
std::shared_ptr<TestModule> TestSprComponents::mpTestModule = nullptr;

void TestSprComponents::SetUpTestCase()
{
    SPR_LOGD("SetUpTestCase enter!");
    mCaseIndex = 0;
    mpEpollSchedule = SprEpollSchedule::GetInstance(0, 2000);
    mpTestModule = std::make_shared<TestModule>();
    mpTestModule->Initialize();

    mpMsgThread = std::make_shared<std::thread>([&]() {
        if (!mpEpollSchedule) {
            SPR_LOGE("mpEpollSchedule is nullptr!");
            return;
        }

        mpEpollSchedule->EpollLoop();
    });

    SPR_LOGD("SetUpTestCase exit!");
}

void TestSprComponents::TearDownTestCase()
{
    SPR_LOGD("TearDownTestCase enter!");
    if (!mpEpollSchedule) {
        SPR_LOGE("mpEpollSchedule is nullptr!");
        return;
    }

    mpTestModule = nullptr;
    mpEpollSchedule->ExitLoop();
    mpMsgThread->join();
    SPR_LOGD("TearDownTestCase exit!");
}
