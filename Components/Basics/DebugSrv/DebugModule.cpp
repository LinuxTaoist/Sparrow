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
#include <list>
#include <memory>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include "PSocket.h"
#include "RemoteShell.h"
#include "DebugModule.h"
#include "SprMediatorIpcProxy.h"

using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("DebugM", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("DebugM", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("DebugM", fmt, ##args)

#define BUFFER_SIZE 1024

DebugModule::DebugModule(ModuleIDType id, const std::string& name)
           : SprObserver(id, name, std::make_shared<SprMediatorIpcProxy>())
{
}

DebugModule::~DebugModule()
{

}

int DebugModule::ProcessMsg(const SprMsg& msg)
{
    switch(msg.GetMsgId())
    {
        case SIG_ID_DEBUG_ENABLE_REMOTE_SHELL:
        {
            MsgRespondEnableRemoteShell(msg);
            break;
        }

        case SIG_ID_DEBUG_DISABLE_REMOTE_SHELL:
        {
            MsgRespondDisableRemoteShell(msg);
            break;
        }

        case SIG_ID_TIMER_ADD_CUSTOM_TIMER:
        case SIG_ID_TIMER_DEL_TIMER:
        {
            MsgRespondDispatchTimerMsg(msg);
            break;
        }

        case SIG_ID_DEBUG_NOTIFY_ALL:
        {
            MsgRespondBroadcastMsg(msg);
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
