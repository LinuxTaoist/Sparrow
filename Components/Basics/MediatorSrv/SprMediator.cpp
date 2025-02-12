/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediator.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/11/25
 *
 *  @Note:
 *  1. In SprMediator, the internal message queue has a fixed name defined by the macro MEDIATOR_MSG_QUEUE.
 *     Other components register own message queues by the name of SprMediator.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <atomic>
#include <errno.h>
#include <string.h>
#include "SprLog.h"
#include "SprSigId.h"
#include "SprDebugNode.h"
#include "CoreTypeDefs.h"
#include "CommonMacros.h"
#include "BindInterface.h"
#include "SprMediator.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("SprMediator", fmt, ##args)
#define SPR_LOGI(fmt, args...) LOGI("SprMediator", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("SprMediator", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("SprMediator", fmt, ##args)

const int32_t MSG_MAX_SIZE = 1024;
static std::atomic<bool> gObjAlive(true);

SprMediator::SprMediator()
{
}

SprMediator::~SprMediator()
{
    gObjAlive = false;
    mMQStatusMap.clear();
    mModuleMap.clear();
}

SprMediator* SprMediator::GetInstance()
{
    if (!gObjAlive) {
        return nullptr;
    }

    static SprMediator instance;
    return &instance;
}

int SprMediator::Init()
{
    SPR_LOGD("### Init SprMediator begin!\n");
    InitInternalPort();
    SprDebugNode::GetInstance()->InitPipeDebugNode(string("/tmp/") + SRV_NAME_MEDIATOR);
    SPR_LOGD("### Init SprMediator end!\n");
    return 0;
}

int SprMediator::InitInternalPort()
{
    mq_unlink(MEDIATOR_MSG_QUEUE);
    mpInternalMQ = make_shared<PMsgQueue>(MEDIATOR_MSG_QUEUE, MSG_MAX_SIZE, [&](int fd, string bytes, void* args) {
        SprMsg msg(bytes);
        ProcessMsg(msg);
        LoadMQDynamicInfo(fd, msg);
    });

    mpInternalMQ->AddToPoll();
    LoadMQStaticInfo(mpInternalMQ->GetEvtFd(), mpInternalMQ->GetMQDevName());   // load mq information of self
    SPR_LOGD("Init internal mq %s fd %d successfully!\n", mpInternalMQ->GetMQDevName().c_str(), mpInternalMQ->GetEvtFd());
    return 0;
}

int SprMediator::GetAllMQStatus(std::vector<SMQStatus> &mqInfoList)
{
    mqInfoList.clear();
    for (const auto& pair : mMQStatusMap) {
        mqInfoList.push_back(pair.second);
    }

    return 0;
}

std::string SprMediator::GetSignalName(int sig)
{
    std::string name = GetSigName(sig);
    return name;
}

int SprMediator::LoadMQStaticInfo(int handle, const std::string& devName)
{
    SMQStatus tmpMQStatus = {};

    tmpMQStatus.handle = handle;
    memset(tmpMQStatus.mqName, 0, sizeof(tmpMQStatus.mqName));
    strncpy(tmpMQStatus.mqName, devName.c_str(), sizeof(tmpMQStatus.mqName) - 1);
    mMQStatusMap[handle] = tmpMQStatus;
    return 0;
}

int SprMediator::LoadMQDynamicInfo(int handle, const SprMsg& msg)
{
    // Avoid receiving SIG_ID_PROXY_REGISTER_REQUEST and reporting errors
    auto mqStatus = mMQStatusMap.find(handle);
    if ( msg.GetMsgId() != SIG_ID_PROXY_REGISTER_REQUEST
      && mqStatus == mMQStatusMap.end()) {
        SPR_LOGE("Not exist mq handle: %d [%s]\n", handle, GetSigName(msg.GetMsgId()));
        return -1;
    }

    // Update mqAttr
    int ret = mq_getattr(handle, &mqStatus->second.mqAttr);
    if (ret != 0) {
        SPR_LOGE("mq_getattr failed! (%s)\n", strerror(errno));
        return -1;
    }

    // Update maxCount
    const mq_attr& attr = mqStatus->second.mqAttr;
    if (attr.mq_curmsgs >= mqStatus->second.maxCount) {
        mqStatus->second.maxCount = attr.mq_curmsgs + 1;
    }

    // Update maxBytes
    if (msg.GetSize() > mqStatus->second.maxBytes) {
        mqStatus->second.maxBytes = msg.GetSize();
    }

    // Update lastMsg, total times
    mqStatus->second.lastMsg = msg.GetMsgId();
    mqStatus->second.total++;

    return 0;
}

int SprMediator::ProcessMsg(const SprMsg& msg)
{
    // SPR_LOGD("[0x%x -> 0x%x] msg: %s\n", msg.GetFrom(), msg.GetTo(), GetSigName(msg.GetMsgId()));
    switch (msg.GetMsgId()) {
        case SIG_ID_PROXY_REGISTER_REQUEST:
            MsgRespondRegister(msg);
            break;

        case SIG_ID_PROXY_UNREGISTER_REQUEST:
            MsgRespondUnregister(msg);
            break;

        default:
            NotifyAllObserver(msg);
            break;
    }

    return 0;
}

int SprMediator::NotifyObserver(ESprModuleID id, const SprMsg& msg)
{
    auto it = mModuleMap.find(id);
    if (it == mModuleMap.end()) {
        SPR_LOGW("Not exist moduleId (0x%x)!\n", id);
        return 0;
    }

    auto& p = it->second.pModMQ;
    if (!p) {
        SPR_LOGW("pModMQ is nullptr! id = 0x%x\n", id);
        return 0;
    }

    string datas;
    msg.Encode(datas);
    int ret = p->Send(datas);
    if (ret < 0) {
        SPR_LOGE("Send failed! (%s)\n", strerror(errno));
    }

    return ret;
}

int SprMediator::NotifyAllObserver(const SprMsg& msg)
{
    for (const auto& pair : mModuleMap) {
        // Skip modules that are unregistered, inactive, or the source of the message
        if (!pair.second.pModMQ || !pair.second.monitored || msg.GetFrom() == pair.first) {
            continue;
        }

        // When destination is NONE, dispatch to all modules.
        // when destination is vaild value, dispatch to the destination
        if (msg.GetTo() == MODULE_NONE || msg.GetTo() == pair.first) {
            NotifyObserver(pair.first, msg);
        }
    }

    return 0;
}

int SprMediator::MsgRespondRegister(const SprMsg& msg)
{
    bool result = false;
    bool monitored = msg.GetBoolValue();
    ESprModuleID moduleId = static_cast<ESprModuleID>(msg.GetU16Value());
    std::string name = msg.GetString();

    if (mModuleMap.find(moduleId) != mModuleMap.end()) {
        SPR_LOGW("Already exist moduleId: 0x%x, reset it\n", moduleId);
    }

    auto pModuleMQ = make_shared<PMsgQueue>(name, MSG_MAX_SIZE, nullptr);
    if (pModuleMQ && pModuleMQ->GetEvtFd() != -1) {
        result = true;
        mModuleMap[moduleId] = {monitored, pModuleMQ};
        LoadMQStaticInfo(pModuleMQ->GetEvtFd(), name);
        LoadMQDynamicInfo(pModuleMQ->GetEvtFd(), msg);
        SPR_LOGD("Register %s success! moduleId = %d, monitored = %d\n", name.c_str(), (int)moduleId, monitored);
    } else {
        SPR_LOGE("Register %s fail!\n", name.c_str());
    }

    SprMsg rspMsg(MODULE_PROXY, moduleId, SIG_ID_PROXY_REGISTER_RESPONSE);
    rspMsg.SetU8Value(result);
    NotifyObserver(moduleId, rspMsg);
    return 0;
}

int SprMediator::MsgRespondUnregister(const SprMsg& msg)
{
    ESprModuleID moduleId = static_cast<ESprModuleID>(msg.GetU16Value());

    auto it = mModuleMap.find(moduleId);
    if (it != mModuleMap.end()) {
        if (it->second.pModMQ) {
            mMQStatusMap.erase(it->second.pModMQ->GetEvtFd());
        }
        mModuleMap.erase(moduleId);
        SPR_LOGD("Unregister module 0x%x success!\n", moduleId);
    } else {
        SPR_LOGW("Not exist module 0x%x\n", moduleId);
    }

    SprMsg exitMsg(SIG_ID_PROXY_BROADCAST_EXIT_COMPONENT);
    exitMsg.SetU32Value(moduleId);
    NotifyAllObserver(exitMsg);
    return 0;
}
