/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DebugModule.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/28
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/28 | 1.0.0.1  | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include "PSocket.h"
#include "SprDebugNode.h"
#include "RemoteShell.h"
#include "DebugModule.h"

using namespace InternalDefs;

#define LOG_TAG "DebugM"

#define BUFFER_SIZE 1024

DebugModule::DebugModule(ModuleIDType id, const std::string& name)
           : SprObserverWithMQueue(id, name)
{
    mSleepSec = 0;
}

DebugModule::~DebugModule()
{
}

int32_t DebugModule::Init()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    p->RegisterCmd(mModuleName, "start1sTimer", "start 1s timer",   std::bind(&DebugModule::DebugStart1sTimer,  this, std::placeholders::_1));
    p->RegisterCmd(mModuleName, "stop1sTimer",  "stop 1s timer",    std::bind(&DebugModule::DebugStop1sTimer,   this, std::placeholders::_1));
    p->RegisterCmd(mModuleName, "sleep",        "sleep",            std::bind(&DebugModule::DebugHandleSleep,   this, std::placeholders::_1));

    return 0;
}

int32_t DebugModule::ProcessMsg(const SprMsg& msg)
{
    switch(msg.GetMsgId()) {
        case SIG_ID_DEBUG_ENABLE_REMOTE_SHELL: {
            MsgRespondEnableRemoteShell(msg);
            break;
        }
        case SIG_ID_DEBUG_DISABLE_REMOTE_SHELL: {
            MsgRespondDisableRemoteShell(msg);
            break;
        }
        case SIG_ID_TIMER_ADD_CUSTOM_TIMER:
        case SIG_ID_TIMER_DEL_TIMER: {
            MsgRespondDispatchTimerMsg(msg);
            break;
        }
        case SIG_ID_DEBUG_NOTIFY_ALL: {
            MsgRespondBroadcastMsg(msg);
            break;
        }
        case SIG_ID_DEBUG_TIMER_TEST_1S: {
            MsgRespondTimer1sTest(msg);
            break;
        }
        case SIG_ID_DEBUG_SLEEP_EVENT: {
            MsgRespondSleepEvent(msg);
            break;
        }
        default:
            SPR_LOGD("msg id: %s\n", GetSigName(msg.GetMsgId()));
            break;
    }

    return 0;
}

int DebugModule::MsgRespondEnableRemoteShell(const SprMsg& msg)
{
    mShell.Enable();
    return 0;
}

int DebugModule::MsgRespondDisableRemoteShell(const SprMsg& msg)
{
    mShell.Disable();
    return 0;
}

int DebugModule::MsgRespondDispatchTimerMsg(const SprMsg& msg)
{
    SprMsg disMsg(msg);
    disMsg.SetTo(MODULE_TIMERM);
    NotifyObserver(disMsg);
    return 0;
}

int DebugModule::MsgRespondBroadcastMsg(const SprMsg& msg)
{
    SprMsg disMsg(msg);
    NotifyAllObserver(disMsg);
    return 0;
}

int DebugModule::MsgRespondTimer1sTest(const SprMsg& msg)
{
    SPR_LOGD("msg id: %s\n", GetSigName(msg.GetMsgId()));
    return 0;
}

int DebugModule::MsgRespondSleepEvent(const SprMsg& msg)
{
    SPR_LOGD("msg id: %s, sleep = %d\n", GetSigName(msg.GetMsgId()), msg.GetI32Value());
    sleep(msg.GetI32Value());
    return 0;
}

void DebugModule::DebugStart1sTimer(const std::vector<std::string>& args)
{
    RegisterTimer(1000, 1000, SIG_ID_DEBUG_TIMER_TEST_1S, 0);
}

void DebugModule::DebugStop1sTimer(const std::vector<std::string>& args)
{
    UnregisterTimer(SIG_ID_DEBUG_TIMER_TEST_1S);
}

void DebugModule::DebugHandleSleep(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        SPR_LOGE("Invalid args size: %d\n", args.size());
        SPR_LOGE("Usage: echo sleep {sec} > /tmp/debugsrv\n");
        return;
    }

    mSleepSec = atoi(args[1].c_str());
    SPR_LOGD("sleep %d\n", mSleepSec);

    SprMsg msg(SIG_ID_DEBUG_SLEEP_EVENT);
    msg.SetI32Value(mSleepSec);
    SendMsg(msg);
}
