/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprTimerManager.cc
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
#include "SprLog.h"
#include "TestModule.h"
#include "SprSigId.h"

using namespace InternalDefs;

#define LOG_TAG "TimerTest"

// 测试200ms永久定时器，等待1100ms后验证计数器值是否为5
TEST_F(TestSprComponents, Timer200MSPermanentWait1100MSVerifyCount)
{
    int32_t waitMs = 1100;
    int32_t expectVal = 5;
    const int32_t& actualVal = mpTestModule->Get200MSCnt();

    SPR_LOGD("[  Case %02d  ] Test200MS_PermanentTimer", ++mCaseIndex);
    mpTestModule->Reset200MSCnt();
    mpTestModule->RegisterTimer(0, 200, SIG_ID_TEST_MODULE_200MS_TIMER_EVENT, 0);
    mpTestModule->CondWait(waitMs, expectVal, actualVal);
    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_200MS_TIMER_EVENT);
    EXPECT_EQ(expectVal, actualVal);
}

// 测试200ms 5次定时器，等待1100ms后验证计数器值是否为5
TEST_F(TestSprComponents, Timer200MS5TimesWait1100MSVerifyCount)
{
    int32_t waitMs = 1100;
    int32_t expectVal = 5;
    const int32_t& actualVal = mpTestModule->Get200MSCnt();

    SPR_LOGD("[  Case %02d  ] Test200MS_5TimesTimer", ++mCaseIndex);
    mpTestModule->Reset200MSCnt();
    mpTestModule->RegisterTimer(0, 200, SIG_ID_TEST_MODULE_200MS_TIMER_EVENT, 5);
    mpTestModule->CondWait(waitMs, expectVal, actualVal);
    EXPECT_EQ(expectVal, actualVal);
}

// 测试500ms永久定时器，等待2600ms后验证计数器值是否为5
TEST_F(TestSprComponents, Timer500MSPermanentWait2600MSVerifyCount)
{
    int32_t waitMs = 2600;
    int32_t expectVal = 5;
    const int32_t& actualVal = mpTestModule->Get500MSCnt();

    SPR_LOGD("[  Case %02d  ] Test500MS_PermanentTimer", ++mCaseIndex);
    mpTestModule->Reset500MSCnt();
    mpTestModule->RegisterTimer(0, 500, SIG_ID_TEST_MODULE_500MS_TIMER_EVENT, 0);
    mpTestModule->CondWait(waitMs, expectVal, actualVal);
    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_500MS_TIMER_EVENT);
    EXPECT_EQ(expectVal, actualVal);
}

// 测试500ms 5次定时器，等待2600ms后验证计数器值是否为5
TEST_F(TestSprComponents, Timer500MS5TimesWait2600MSVerifyCount)
{
    int32_t waitMs = 2600;
    int32_t expectVal = 5;
    const int32_t& actualVal = mpTestModule->Get500MSCnt();

    SPR_LOGD("[  Case %02d  ] Test500MS_5TimesTimer", ++mCaseIndex);
    mpTestModule->Reset500MSCnt();
    mpTestModule->RegisterTimer(0, 500, SIG_ID_TEST_MODULE_500MS_TIMER_EVENT, 5);
    mpTestModule->CondWait(waitMs, expectVal, actualVal);
    EXPECT_EQ(expectVal, actualVal);
}

// 测试1s永久定时器，等待2100ms后验证计数器值是否为2
TEST_F(TestSprComponents, Timer1SPermanentWait2100MSVerifyCount)
{
    int32_t waitMs = 2100;
    int32_t expectVal = 2;
    const int32_t& actualVal = mpTestModule->Get1SCnt();

    SPR_LOGD("[  Case %02d  ] Test1S_PermanentTimer", ++mCaseIndex);
    mpTestModule->Reset1SCnt();
    mpTestModule->RegisterTimer(0, 1000, SIG_ID_TEST_MODULE_1S_TIMER_EVENT, 0);
    mpTestModule->CondWait(waitMs, expectVal, actualVal);
    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_1S_TIMER_EVENT);
    EXPECT_EQ(expectVal, actualVal);
}

// 测试1s 2次定时器，等待3000ms后验证计数器值是否为2
TEST_F(TestSprComponents, Timer1S2TimesWait3000MSVerifyCount)
{
    int32_t waitMs = 3000;
    int32_t expectVal = 2;
    const int32_t& actualVal = mpTestModule->Get1SCnt();

    SPR_LOGD("[  Case %02d  ] Test1S_2TimesTimer", ++mCaseIndex);
    mpTestModule->Reset1SCnt();
    mpTestModule->RegisterTimer(0, 1000, SIG_ID_TEST_MODULE_1S_TIMER_EVENT, 2);
    mpTestModule->CondWait(waitMs, expectVal, actualVal);
    EXPECT_EQ(expectVal, actualVal);
}

// 测试2s永久定时器，等待4100ms后验证计数器值是否为2
TEST_F(TestSprComponents, Timer2SPermanentWait4100MSVerifyCount)
{
    int32_t waitMs = 4100;
    int32_t expectVal = 2;
    const int32_t& actualVal = mpTestModule->Get2SCnt();

    SPR_LOGD("[  Case %02d  ] Test2S_PermanentTimer", ++mCaseIndex);
    mpTestModule->Reset2SCnt();
    mpTestModule->RegisterTimer(0, 2000, SIG_ID_TEST_MODULE_2S_TIMER_EVENT, 0);
    mpTestModule->CondWait(waitMs, expectVal, actualVal);
    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_2S_TIMER_EVENT);
    EXPECT_EQ(expectVal, actualVal);
}

// 测试2s 2次定时器，等待4100ms后验证计数器值是否为2
TEST_F(TestSprComponents, Timer2S2TimesWait4100MSVerifyCount)
{
    int32_t waitMs = 4100;
    int32_t expectVal = 2;
    const int32_t& actualVal = mpTestModule->Get2SCnt();

    SPR_LOGD("[  Case %02d  ] Test2S_2TimesTimer", ++mCaseIndex);
    mpTestModule->Reset2SCnt();
    mpTestModule->RegisterTimer(0, 2000, SIG_ID_TEST_MODULE_2S_TIMER_EVENT, 2);
    mpTestModule->CondWait(waitMs, expectVal, actualVal);
    EXPECT_EQ(expectVal, actualVal);
}

// 测试多个定时器交叉使用
TEST_F(TestSprComponents, TimerMultipleTimersCrossUse)
{
    int32_t waitMs = 2100;
    int32_t expectVal200MS = 10;
    int32_t expectVal500MS = 4;
    int32_t expectVal1S = 2;
    const int32_t& actualVal200MS = mpTestModule->Get200MSCnt();
    const int32_t& actualVal500MS = mpTestModule->Get500MSCnt();
    const int32_t& actualVal1S = mpTestModule->Get1SCnt();

    SPR_LOGD("[  Case %02d  ] TestMultipleTimersCrossUse", ++mCaseIndex);
    mpTestModule->Reset200MSCnt();
    mpTestModule->Reset500MSCnt();
    mpTestModule->Reset1SCnt();

    mpTestModule->RegisterTimer(0, 200, SIG_ID_TEST_MODULE_200MS_TIMER_EVENT, 0);
    mpTestModule->RegisterTimer(0, 500, SIG_ID_TEST_MODULE_500MS_TIMER_EVENT, 0);
    mpTestModule->RegisterTimer(0, 1000, SIG_ID_TEST_MODULE_1S_TIMER_EVENT, 0);

    mpTestModule->CondWait(waitMs, expectVal200MS, actualVal200MS);
    mpTestModule->CondWait(waitMs, expectVal500MS, actualVal500MS);
    mpTestModule->CondWait(waitMs, expectVal1S, actualVal1S);

    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_200MS_TIMER_EVENT);
    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_500MS_TIMER_EVENT);
    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_1S_TIMER_EVENT);

    EXPECT_EQ(expectVal200MS, actualVal200MS);
    EXPECT_EQ(expectVal500MS, actualVal500MS);
    EXPECT_EQ(expectVal1S, actualVal1S);
}

// 测试先注册多个定时器，然后注销部分定时器
TEST_F(TestSprComponents, TimerRegisterMultiUnregisterPart)
{
    int32_t waitMsFirst = 1100;
    int32_t waitMsSecond = 1100;
    int32_t expectVal200MSFirst = 5;
    int32_t expectVal200MSSecond = 10;
    int32_t expectVal500MSFirst = 4;
    int32_t expectVal500MSSecond = 0;
    const int32_t& actualVal200MSFirst = mpTestModule->Get200MSCnt();
    const int32_t& actualVal500MSFirst = mpTestModule->Get500MSCnt();
    const int32_t& actualVal200MSSecond = mpTestModule->Get200MSCnt();

    SPR_LOGD("[  Case %02d  ] TestRegisterMultipleAndUnregisterPart", ++mCaseIndex);
    mpTestModule->Reset200MSCnt();
    mpTestModule->Reset500MSCnt();

    mpTestModule->RegisterTimer(0, 200, SIG_ID_TEST_MODULE_200MS_TIMER_EVENT, 0);
    mpTestModule->RegisterTimer(0, 500, SIG_ID_TEST_MODULE_500MS_TIMER_EVENT, 0);

    mpTestModule->CondWait(waitMsFirst, expectVal200MSFirst, actualVal200MSFirst);  // wait 1100ms
    mpTestModule->CondWait(waitMsFirst, expectVal500MSFirst, actualVal500MSFirst);  // wait 1100ms

    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_500MS_TIMER_EVENT);

    mpTestModule->CondWait(waitMsSecond, expectVal200MSSecond, actualVal200MSSecond); // wait 1100ms

    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_200MS_TIMER_EVENT);

    EXPECT_EQ(expectVal200MSSecond, actualVal200MSSecond);
    EXPECT_EQ(expectVal500MSFirst + expectVal500MSSecond, actualVal500MSFirst);
}

// 测试多个定时器顺序启动和停止
TEST_F(TestSprComponents, TimerMultipleTimersSeqStartStop)
{
    int32_t waitMs1 = 1100;
    int32_t waitMs2 = 1100;
    int32_t waitMs3 = 1100;
    int32_t expectVal200MS = 15;
    int32_t expectVal500MS = 4;
    int32_t expectVal1S = 1;
    const int32_t& actualVal200MS = mpTestModule->Get200MSCnt();
    const int32_t& actualVal500MS = mpTestModule->Get500MSCnt();
    const int32_t& actualVal1S = mpTestModule->Get1SCnt();

    SPR_LOGD("[  Case %02d  ] TestTimersSequentialStartStop", ++mCaseIndex);
    mpTestModule->Reset200MSCnt();
    mpTestModule->Reset500MSCnt();
    mpTestModule->Reset1SCnt();

    mpTestModule->RegisterTimer(200, 200, SIG_ID_TEST_MODULE_200MS_TIMER_EVENT, 0);
    mpTestModule->CondWait(waitMs1, expectVal200MS / 3, actualVal200MS);            // wait 1000ms

    mpTestModule->RegisterTimer(500, 500, SIG_ID_TEST_MODULE_500MS_TIMER_EVENT, 0);
    mpTestModule->CondWait(waitMs2, expectVal500MS / 2, actualVal500MS);            // wait 1000ms

    mpTestModule->RegisterTimer(1000, 1000, SIG_ID_TEST_MODULE_1S_TIMER_EVENT, 0);
    mpTestModule->CondWait(waitMs3, expectVal1S, actualVal1S);                      // wait 1000ms

    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_200MS_TIMER_EVENT);
    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_500MS_TIMER_EVENT);
    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_1S_TIMER_EVENT);

    EXPECT_EQ(expectVal200MS, actualVal200MS);
    EXPECT_EQ(expectVal500MS, actualVal500MS);
    EXPECT_EQ(expectVal1S, actualVal1S);
}

// 测试多个定时器交替启动和停止
TEST_F(TestSprComponents, TimerMultipleTimersAltStartStop)
{
    int32_t waitMs = 510;
    int32_t expectVal200MS = 2;
    int32_t expectVal500MS = 1;
    int32_t expectVal1S = 0;
    const int32_t& actualVal200MS = mpTestModule->Get200MSCnt();
    const int32_t& actualVal500MS = mpTestModule->Get500MSCnt();
    const int32_t& actualVal1S = mpTestModule->Get1SCnt();

    SPR_LOGD("[  Case %02d  ] TestTimersAlternateStartStop", ++mCaseIndex);
    mpTestModule->Reset200MSCnt();
    mpTestModule->Reset500MSCnt();
    mpTestModule->Reset1SCnt();

    mpTestModule->RegisterTimer(0, 200, SIG_ID_TEST_MODULE_200MS_TIMER_EVENT, 0);
    mpTestModule->CondWait(waitMs, expectVal200MS, actualVal200MS);         // wait 510ms
    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_200MS_TIMER_EVENT);

    mpTestModule->RegisterTimer(0, 500, SIG_ID_TEST_MODULE_500MS_TIMER_EVENT, 0);
    mpTestModule->CondWait(waitMs, expectVal500MS, actualVal500MS);         // wait 510ms
    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_500MS_TIMER_EVENT);

    mpTestModule->RegisterTimer(0, 1000, SIG_ID_TEST_MODULE_1S_TIMER_EVENT, 0);
    mpTestModule->CondWait(waitMs, expectVal1S, actualVal1S);               // wait 510ms
    mpTestModule->UnregisterTimer(SIG_ID_TEST_MODULE_1S_TIMER_EVENT);

    EXPECT_EQ(expectVal200MS, actualVal200MS);
    EXPECT_EQ(expectVal500MS, actualVal500MS);
    EXPECT_EQ(expectVal1S, actualVal1S);
}