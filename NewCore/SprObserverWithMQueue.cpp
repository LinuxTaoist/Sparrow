/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprObserverWithMQueue.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/15
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/08/15 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "SprLog.h"
#include "PMsgQueue.h"
#include "SprMediatorMQProxy.h"
#include "SprObserverWithMQueue.h"

using namespace InternalDefs;

#define SPR_LOGD(fmt, args...)  LOGD("SprObsMQ", "[%s] " fmt, mModuleName.c_str(), ##args)
#define SPR_LOGE(fmt, args...)  LOGE("SprObsMQ", "[%s] " fmt, mModuleName.c_str(), ##args)

SprObserverWithMQueue::SprObserverWithMQueue(ModuleIDType id, const std::string& name, EProxyType proxyType)
    : SprObserver(id, name, proxyType), PMsgQueue(name), mConnected(false)
{
}

SprObserverWithMQueue::~SprObserverWithMQueue()
{
}

int32_t SprObserverWithMQueue::SendMsg(SprMsg& msg)
{
    std::string bytes;
    msg.SetFrom(mModuleID);
    msg.SetTo(mModuleID);
    msg.Encode(bytes);

    int32_t ret = Send(bytes);
    if (ret < 0) {
        SPR_LOGE("Send failed!\n");
    }

    return ret;
}

int32_t SprObserverWithMQueue::RecvMsg(SprMsg& msg)
{
    uint32_t prio = 0;
    std::string bytes;
    int32_t ret = Recv(bytes, prio);
    if (ret < 0) {
        SPR_LOGE("Recv failed!");
        return -1;
    }

    return msg.Decode(bytes);
}

int SprObserverWithMQueue::MsgResponseSystemExitRsp(const SprMsg& msg)
{
    SPR_LOGD("System Exit!\n");
    MainExit();
    return 0;
}

int SprObserverWithMQueue::MsgResponseRegisterRsp(const SprMsg& msg)
{
    SPR_LOGD("Register Successfully!\n");
    mConnected = msg.GetU8Value();
    return 0;
}

int SprObserverWithMQueue::MsgResponseUnregisterRsp(const SprMsg& msg)
{
    // 注销成功，连接状态为false
    SPR_LOGD("Unregister Successfully!\n");
    mConnected = !msg.GetU8Value();
    return 0;
}

void* SprObserverWithMQueue::EpollEvent(int fd, EpollType eType, void* arg)
{
    SprMsg msg;
    if (RecvMsg(msg) < 0) {
        SPR_LOGE("RecvMsg failed!\n");
        return nullptr;
    }

    switch (msg.GetMsgId())
    {
        case SIG_ID_PROXY_REGISTER_RESPONSE:
        {
            MsgResponseRegisterRsp(msg);
            break;
        }
        case SIG_ID_PROXY_UNREGISTER_RESPONSE:
        {
            MsgResponseUnregisterRsp(msg);
            break;
        }
        case SIG_ID_SYSTEM_EXIT:
        {
            MsgResponseSystemExitRsp(msg);
            break;
        }
        default:
        {
            ProcessMsg(msg);
            break;
        }
    }

    return nullptr;
}

