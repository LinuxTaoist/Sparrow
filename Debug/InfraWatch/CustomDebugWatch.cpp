/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CustomDebugWatch.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/04/23
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/04/23 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "InfraCommon.h"
#include "CoreTypeDefs.h"
#include "GeneralUtils.h"
#include "CustomDebugWatch.h"
#include "SprMediatorIpcProxy.h"

using namespace std;
using namespace InternalDefs;
using namespace GeneralUtils;

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

CustomDebugWatch theCustomDebugWatch;

CustomDebugWatch::CustomDebugWatch()
{
    mDebugModulePtr = std::make_shared<DebugModule>(MODULE_DEBUG_WATCH, "DebugW", std::make_shared<SprMediatorIpcProxy>());

    if (!mEpollThread.joinable()) {
        mEpollThread = std::thread([]{
            SprObserver::MainLoop();
        });
    }
}

CustomDebugWatch::~CustomDebugWatch()
{
    if (mEpollThread.joinable()) {
        SprObserver::MainExit();
        mEpollThread.join();
    }
}

char CustomDebugWatch::MenuEntry()
{
    InfraWatch::ClearScreen();

    SPR_LOG("================================  Custom  Debug  ================================\n"
            "\n"
            "    1. AddTimerInOneSec \n"
            "    2. DelTimerInOneSec \n"
            "    3. AddCustomTimer   \n"
            "    4. DelCustomTimer   \n"
            "\n"
            "    [Q] Quit\n"
            "\n"
            "=================================================================================\n");

    char input = InfraWatch::WaitUserInputWithoutEnter();
    HandleInputInMenu(input);
    return input;
}

char CustomDebugWatch::HandleInputInMenu(char input)
{
    switch(input)
    {
        case '1':
        {
            mDebugModulePtr->AddTimerInOneSec();
            break;
        }

        case '2':
        {
            mDebugModulePtr->DelTimerInOneSec();
            break;
        }

        case '3':
        {
            mDebugModulePtr->AddCustomTimer();
            break;
        }

        case '4':
        {
            mDebugModulePtr->DelCustomTimer();
            break;
        }

        case 'q':
        {
            break;
        }

        default:
            break;
    }
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------
// Debug module functions
//---------------------------------------------------------------------------------------------------------------------

DebugModule::DebugModule(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr)
    : SprObserver(id, name, mMsgMediatorPtr)
{

}

DebugModule::~DebugModule()
{

}

int DebugModule::ProcessMsg(const SprMsg& msg)
{
    switch(msg.GetMsgId())
    {
        default:
            SPR_LOG("msg id: %s %s\n", GetSigName(msg.GetMsgId()), GetCurTimeStr().c_str());
            break;
    }

    return 0;
}

int DebugModule::AddTimerInOneSec()
{
    STimerInfo timeInfo = {MODULE_DEBUG_WATCH, SIG_ID_DEBUG_TIMER_TEST_1S, 0, 0, 1000};
    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));
    SprMsg msg(MODULE_TIMERM, SIG_ID_TIMER_ADD_CUSTOM_TIMER);

    msg.SetDatas(pInfo, sizeof(STimerInfo));
    NotifyObserver(msg);
    return 0;
}

int DebugModule::DelTimerInOneSec()
{
    STimerInfo timeInfo = {MODULE_DEBUG_WATCH, SIG_ID_DEBUG_TIMER_TEST_1S, 0, 0, 0};
    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));
    SprMsg msg(MODULE_TIMERM, SIG_ID_TIMER_DEL_TIMER);

    msg.SetDatas(pInfo, sizeof(STimerInfo));
    NotifyObserver(msg);
    return 0;
}

int DebugModule::AddCustomTimer()
{
    uint32_t repeatTimes = 0;
    int32_t  delayInMilliSec, intervalInMilliSec;
    SPR_LOG("Input timer settings: <repetitions> <delay ms> <interval ms> \n");
    int ret = scanf("%d %d %d", &repeatTimes, &delayInMilliSec, &intervalInMilliSec);
    if (ret != 3) {
        SPR_LOG("Invalid input! (%s)\n", strerror(errno));
        return -1;
    }

    STimerInfo timeInfo = {MODULE_DEBUG_WATCH, SIG_ID_DEBUG_TIMER_TEST, repeatTimes, delayInMilliSec, intervalInMilliSec};
    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));
    SprMsg msg(MODULE_TIMERM, SIG_ID_TIMER_ADD_CUSTOM_TIMER);

    msg.SetDatas(pInfo, sizeof(STimerInfo));
    NotifyAllObserver(msg);
    return 0;
}

int DebugModule::DelCustomTimer()
{
    STimerInfo timeInfo = {MODULE_DEBUG_WATCH, SIG_ID_DEBUG_TIMER_TEST, 0, 0, 0};
    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));
    SprMsg msg(MODULE_TIMERM, SIG_ID_TIMER_DEL_TIMER);

    msg.SetDatas(pInfo, sizeof(STimerInfo));
    NotifyObserver(msg);
    return 0;
}
