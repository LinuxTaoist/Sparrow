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
#include <errno.h>
#include <string.h>
#include "SprLog.h"
#include "PMsgQueue.h"
#include "GeneralUtils.h"
#include "RunningTiming.h"
#include "CommonErrorCodes.h"
#include "SprObserverWithMQueue.h"

using namespace InternalDefs;
using namespace GeneralUtils;

#define LOG_TAG "SprObsMQ"

#define MSG_SIZE_MAX    1025
#define RUNTIME_WARN_MS 2000

SprObserverWithMQueue::SprObserverWithMQueue(ModuleIDType id, const std::string& name, EProxyType proxyType)
    : SprObserver(id, name, proxyType), PMsgQueue(name + "_" + GetRandomString(8), MSG_SIZE_MAX), mConnected(false)
{
    mpDetails = std::make_shared<SprMQueueDetails>(GetMQDevName(), true);
}

SprObserverWithMQueue::~SprObserverWithMQueue()
{
    UnRegisterFromMediator();
}

int32_t SprObserverWithMQueue::InitFramework()
{
    SPR_LOGD("Initlize MQueue framework!\n");
    AddToPoll();
    LoadMQStaticInfo(GetEvtFd(), GetMQDevName());
    return RegisterFromMediator();
}

int32_t SprObserverWithMQueue::RegisterFromMediator()
{
    SprMsg msg(GetModuleId(), MODULE_PROXY, SIG_ID_PROXY_REGISTER_REQUEST);
    msg.SetBoolValue(true);
    msg.SetU32Value((uint32_t)MEDIATOR_PROXY_MQUEUE);
    msg.SetU16Value((uint16_t)GetModuleId());
    msg.SetString(GetMQDevName());
    return NotifyObserver(msg);
}

int32_t SprObserverWithMQueue::UnRegisterFromMediator()
{
    SprMsg msg(GetModuleId(), MODULE_PROXY, SIG_ID_PROXY_UNREGISTER_REQUEST);
    msg.SetU32Value((uint32_t)MEDIATOR_PROXY_MQUEUE);
    msg.SetU16Value((uint16_t)GetModuleId());
    msg.SetString(GetMQDevName());
    return NotifyObserver(msg);
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

int32_t SprObserverWithMQueue::SendMsg(uint32_t msgId)
{
    SprMsg msg(msgId);
    return SendMsg(msg);
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

int32_t SprObserverWithMQueue::MsgRespondSystemExitRsp(const SprMsg& msg)
{
    SPR_LOGD("System Exit!\n");
    return 0;
}

int32_t SprObserverWithMQueue::MsgRespondRegisterRsp(const SprMsg& msg)
{
    // 注册成功，连接状态为true
    mConnected = msg.GetU8Value();
    SPR_LOGD("Register Successfully! mConnected = %d\n", mConnected);
    return 0;
}

int32_t SprObserverWithMQueue::MsgRespondUnregisterRsp(const SprMsg& msg)
{

    mConnected = !msg.GetU8Value();
    SPR_LOGD("Unregister Successfully! mConnected = %d\n", mConnected);
    return 0;
}

int32_t SprObserverWithMQueue::MsgRespondPropertyChangedRsp(const SprMsg& msg)
{
    const std::string text = msg.GetString();
    const size_t eqPos = text.find('=');

    if (eqPos == std::string::npos || text.substr(0, eqPos) != PROPERTY_KEY_LOG_LEVEL) {
        return 0;
    }

    char* endptr;
    const int32_t level = strtol(text.substr(eqPos + 1).c_str(), &endptr, 10);
    if (*endptr != '\0' || level < InternalDefs::LOG_LEVEL_MIN || level > InternalDefs::LOG_LEVEL_BUTT) {
        return 0;
    }

    const int32_t oldLevel = SprLog::GetInstance()->GetLevel();
    if (level != oldLevel) {
        SprLog::GetInstance()->SetLevel(level);
        SPR_LOGD("Log level changed! %d -> %d\n", oldLevel, level);
    }

    return 0;
}


int32_t SprObserverWithMQueue::LoadMQStaticInfo(int32_t handle, const std::string& devName)
{
    if (devName.length() >= MQ_NAME_MAX_LENGTH) {
        SPR_LOGW("devName %s too long(max %d characters)\n", devName.c_str(), MQ_NAME_MAX_LENGTH);
    }

    if (!mpDetails) {
        SPR_LOGE("mpDetails is nullptr!\n");
        return -1;
    }

    mpDetails->SetHandle(handle);
    return 0;
}

int32_t SprObserverWithMQueue::LoadMQDynamicInfo(int32_t handle, const SprMsg& msg)
{
    if (!mpDetails) {
        SPR_LOGE("mpDetails is nullptr!\n");
        return -1;
    }

    mq_attr tmpMQAttr = {};
    int32_t ret = mq_getattr(handle, &tmpMQAttr);
    if (ret != 0) {
        SPR_LOGE("mq_getattr failed! (%s)\n", strerror(errno));
        return -1;
    }

    int32_t usedPeak = 0;
    mpDetails->GetUsedPeak(usedPeak);
    if (tmpMQAttr.mq_curmsgs + 1 >= usedPeak) {
        mpDetails->SetUsedPeak(tmpMQAttr.mq_curmsgs + 1);
    }

    int32_t msgLenPeak = 0;
    mpDetails->GetMsgLenPeak(msgLenPeak);
    if (msg.GetSize() > msgLenPeak) {
        mpDetails->SetMsgLenPeak(msg.GetSize());
    }

    mpDetails->SetLastMsgID(msg.GetMsgId());
    mpDetails->IncrementMsgTotal();
    return 0;
}

int32_t SprObserverWithMQueue::SendEventToMonitor(int32_t errcode, const std::string& text)
{
    SPR_LOGD("Send event to monitor, errcode: %d, text: %s\n", errcode, text.c_str());
    SprMsg msg(SIG_ID_MONITOR_STATUS_EVENT);
    msg.SetI32Value(errcode);
    msg.SetString(text);
    return NotifyObserver(MODULE_STATUS_MONITOR, msg);
}

int32_t SprObserverWithMQueue::DispatchSprMsg(const SprMsg& msg)
{
    RunningTiming timer;
    switch (msg.GetMsgId()) {
        case SIG_ID_PROXY_REGISTER_RESPONSE: {
            MsgRespondRegisterRsp(msg);
            break;
        }
        case SIG_ID_PROXY_UNREGISTER_RESPONSE: {
            MsgRespondUnregisterRsp(msg);
            break;
        }
        case SIG_ID_SYSTEM_EXIT: {
            MsgRespondSystemExitRsp(msg);
            break;
        }
        case SIG_ID_PROPERTY_CHANGED:   // Not break, only deal with property "loglevel"
            MsgRespondPropertyChangedRsp(msg);
            // fall through
        default: {
            ProcessMsg(msg);
            break;
        }
    }

    uint64_t estime = timer.GetElapsedTimeInMSec();
    if (estime >= RUNTIME_WARN_MS) {
        std::string description = std::string(GetSigName(msg.GetMsgId())) + " took "
            + std::to_string(estime) + "ms" + " (limit: " + std::to_string(RUNTIME_WARN_MS) + "ms" + ")";
        SendEventToMonitor(ERR_GENERAL_RUN_LONGTIME, description);
    }

    // SPR_LOGD("Dispatch SprMsg %s time: %dms\n", GetSigName(msg.GetMsgId()), estime);
    return 0;
}

void* SprObserverWithMQueue::EpollEvent(int fd, EpollType eType, void* arg)
{
    if (fd != GetEvtFd()) {
        SPR_LOGW("fd is not match!\n");
        return nullptr;
    }

    SprMsg msg;
    if (RecvMsg(msg) < 0) {
        SPR_LOGE("RecvMsg failed!\n");
        return nullptr;
    }

    LoadMQDynamicInfo(fd, msg);
    DispatchSprMsg(msg);
    return nullptr;
}
