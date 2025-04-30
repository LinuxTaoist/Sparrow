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
#include "SprEpollSchedule.h"
#include "SprObserverWithMQueue.h"

class TestModule : public SprObserverWithMQueue
{
public:
    TestModule();
    virtual ~TestModule();

    int32_t CondNotify();
    int32_t CondWait(int32_t timeoutMs);
    int32_t CondWait(int32_t timeoutMs, int32_t expectValue, const int32_t& actualValue);

    void Reset200MSCnt() { m200MSCnt = 0; }
    void Reset500MSCnt() { m500MSCnt = 0; }
    void Reset1SCnt() { m1SCnt = 0; }
    void Reset2SCnt() { m2SCnt = 0; }
    void Reset3SCnt() { m3SCnt = 0; }
    void Reset5SCnt() { m5SCnt = 0; }
    const int32_t& Get200MSCnt() { return m200MSCnt; }
    const int32_t& Get500MSCnt() { return m500MSCnt; }
    const int32_t& Get1SCnt() { return m1SCnt; }
    const int32_t& Get2SCnt() { return m2SCnt; }
    const int32_t& Get3SCnt() { return m3SCnt; }
    const int32_t& Get5SCnt() { return m5SCnt; }

private:
    int32_t Init() override;
    int32_t ProcessMsg(const SprMsg& msg) override;

private:
    int32_t m200MSCnt;
    int32_t m500MSCnt;
    int32_t m1SCnt;
    int32_t m2SCnt;
    int32_t m3SCnt;
    int32_t m5SCnt;
    std::mutex mMutex;
    std::condition_variable mCond;
};

class TestSprComponents : public ::testing::Test {
protected:
    static void SetUpTestCase();
    static void TearDownTestCase();

public:
    static int32_t mCaseIndex;
    static EpollEventHandler* mpEpollSchedule;
    static std::shared_ptr<std::thread> mpMsgThread;
    static std::shared_ptr<TestModule> mpTestModule;
};

#endif // __TEST_MODULE_H__
