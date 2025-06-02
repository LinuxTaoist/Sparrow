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
#include "SprEnumHelper.h"
#include "SprMediator.h"

using namespace std;
using namespace InternalDefs;

#define LOG_TAG "SprMediator"

const int32_t MSG_MAX_SIZE = 1024;
static std::atomic<bool> gObjAlive(true);

SprMediator::SprMediator()
{
}

SprMediator::~SprMediator()
{
    gObjAlive = false;
    mMQDetailsMap.clear();
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
    SPR_LOGD("### Init SprMediator end!\n");
    return 0;
}

int SprMediator::InitInternalPort()
{
    mq_unlink(MEDIATOR_MSG_QUEUE);
    mpInternalMQ = make_shared<PMsgQueue>(MEDIATOR_MSG_QUEUE, MSG_MAX_SIZE, [&](int fd, string bytes, void* args) {
        SprMsg msg(bytes);
        ProcessMsg(msg);
    });

    mpInternalMQ->AddToPoll();
    SPR_LOGD("Init internal mq %s fd %d successfully!\n", mpInternalMQ->GetMQDevName().c_str(), mpInternalMQ->GetEvtFd());
    return 0;
}

int SprMediator::GetAllMQStatus(std::vector<SMQueueDetails> &mqInfoList)
{
    mqInfoList.clear();
    for (const auto& pair : mMQDetailsMap) {
        SMQueueDetails details = {};
        pair.second->GetMQDetails(details);
        mqInfoList.push_back(details);
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
    auto pMQDetails = std::make_shared<SprMQueueDetails>(devName, false);
    pMQDetails->SetHandle(handle);
    mMQDetailsMap[handle] = pMQDetails;
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
    if (pModuleMQ->GetEvtFd() != -1) {
        result = true;
        mModuleMap[moduleId] = {monitored, pModuleMQ};
        LoadMQStaticInfo(pModuleMQ->GetEvtFd(), name);
        SPR_LOGD("Register %s success! %s [%d], monitored = %d\n",
                name.c_str(), GetSprModuleIDDescription(moduleId).c_str(), (int)moduleId, monitored);
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
            mMQDetailsMap.erase(it->second.pModMQ->GetEvtFd());
        }
        mModuleMap.erase(moduleId);
        SPR_LOGD("Unregister module %s success!\n", GetSprModuleIDDescription(moduleId).c_str());
    } else {
        SPR_LOGW("Not exist module %s\n", GetSprModuleIDDescription(moduleId).c_str());
    }

    SprMsg exitMsg(SIG_ID_PROXY_BROADCAST_EXIT_COMPONENT);
    exitMsg.SetU32Value(moduleId);
    NotifyAllObserver(exitMsg);
    return 0;
}
