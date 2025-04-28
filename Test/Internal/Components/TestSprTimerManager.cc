/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprTimer.cc
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
#include "TestModule.h"
#include "SprSigId.h"

using namespace InternalDefs;

#define LOG_TAG "TimerTest"

TEST_F(TestSprComponents, Test200MSTimerCase1)
{
    int32_t expectVal = 5;
    int32_t waitMs = 1100;

    mpTestModule->ResetTimerCnt();
    mpTestModule->RegisterTimer(0, 200, SIG_ID_TEST_MODULE_200MS_TIMER_EVENT, 0);
    mpTestModule->CondWait(waitMs, expectVal);
    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_200MS_TIMER_EVENT);
    EXPECT_EQ(expectVal, mpTestModule->GetTimerCnt());
}

TEST_F(TestSprComponents, Test200MSTimerCase2)
{
    int32_t expectVal = 5;
    int32_t waitMs = 1100;

    mpTestModule->ResetTimerCnt();
    mpTestModule->RegisterTimer(0, 200, SIG_ID_TEST_MODULE_200MS_TIMER_EVENT, 5);
    mpTestModule->CondWait(waitMs, expectVal);
    EXPECT_EQ(expectVal, mpTestModule->GetTimerCnt());
}

TEST_F(TestSprComponents, Test500MSTimerCase3)
{
    int32_t expectVal = 5;
    int32_t waitMs = 2600;

    mpTestModule->ResetTimerCnt();
    mpTestModule->RegisterTimer(0, 500, SIG_ID_TEST_MODULE_500MS_TIMER_EVENT, 0);
    mpTestModule->CondWait(waitMs, expectVal);
    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_500MS_TIMER_EVENT);
    EXPECT_EQ(expectVal, mpTestModule->GetTimerCnt());
}

TEST_F(TestSprComponents, Test500MSTimerCase4)
{
    int32_t expectVal = 5;
    int32_t waitMs = 2600;

    mpTestModule->ResetTimerCnt();
    mpTestModule->RegisterTimer(0, 500, SIG_ID_TEST_MODULE_500MS_TIMER_EVENT, 5);
    mpTestModule->CondWait(waitMs, expectVal);
    EXPECT_EQ(expectVal, mpTestModule->GetTimerCnt());
}

// 1s 永久性定时器
TEST_F(TestSprComponents, Test1STimerCase5)
{
    int32_t expectVal = 2;
    int32_t waitMs = 2100;

    mpTestModule->ResetTimerCnt();
    mpTestModule->RegisterTimer(0, 1000, SIG_ID_TEST_MODULE_1S_TIMER_EVENT, 0);
    mpTestModule->CondWait(waitMs, expectVal);
    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_1S_TIMER_EVENT);
    EXPECT_EQ(expectVal, mpTestModule->GetTimerCnt());
}

// 1s 5次定时器
TEST_F(TestSprComponents, Test1STimerCase6)
{
    int32_t expectVal = 2;
    int32_t waitMs = 3000;

    mpTestModule->ResetTimerCnt();
    mpTestModule->RegisterTimer(0, 1000, SIG_ID_TEST_MODULE_1S_TIMER_EVENT, 2);
    mpTestModule->CondWait(waitMs, expectVal);
    EXPECT_EQ(expectVal, mpTestModule->GetTimerCnt());
}

// 2s 永久性定时器
TEST_F(TestSprComponents, Test2STimerCase7)
{
    int32_t expectVal = 2;
    int32_t waitMs = 4100;

    mpTestModule->ResetTimerCnt();
    mpTestModule->RegisterTimer(0, 2000, SIG_ID_TEST_MODULE_2S_TIMER_EVENT, 0);
    mpTestModule->CondWait(waitMs, expectVal);
    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_1S_TIMER_EVENT);
    EXPECT_EQ(expectVal, mpTestModule->GetTimerCnt());
}

// 2s 2次定时器
TEST_F(TestSprComponents, Test2STimerCase8)
{
    int32_t expectVal = 2;
    int32_t waitMs = 4100;

    mpTestModule->ResetTimerCnt();
    mpTestModule->RegisterTimer(0, 2000, SIG_ID_TEST_MODULE_2S_TIMER_EVENT, 2);
    mpTestModule->CondWait(waitMs, expectVal);
    EXPECT_EQ(expectVal, mpTestModule->GetTimerCnt());
}
