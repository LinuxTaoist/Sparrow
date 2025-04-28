/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestModule.h
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
#ifndef __TEST_MODULE_H__
#define __TEST_MODULE_H__

#include <thread>
#include <memory>
#include <condition_variable>
#include "gtest/gtest.h"
#include "SprObserverWithMQueue.h"

class TestModule : public SprObserverWithMQueue
{
public:
    TestModule();
    virtual ~TestModule();

    void InitEnv();
    void DeInitEnv();
    int32_t GetTimerCnt() { return mTimerCount; }
    int32_t ResetTimerCnt();
    int32_t CondNotify();
    int32_t CondWait(int32_t timeoutMs, int32_t value);

private:
    int32_t Init() override;
    int32_t ProcessMsg(const SprMsg& msg) override;

private:
    int32_t mTimerCount;
    std::mutex mMutex;
    std::condition_variable mCond;
    std::shared_ptr<std::thread> mpMsgThread;
};

class TestSprComponents : public ::testing::Test {
protected:
    static void SetUpTestCase();
    static void TearDownTestCase();

public:
    static std::shared_ptr<TestModule> mpTestModule;
};

#endif // __TEST_MODULE_H__
