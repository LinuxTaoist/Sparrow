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
const std::string MEDIATOR_LABEL = "Mediator";

SprMediator::SprMediator()
{
}

SprMediator::~SprMediator()
{
    gObjAlive = false;
    mMQDetailsMap.clear();
    mModuleMap.clear();
    UnregisterDebugFuncs();
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
    RegisterDebugFuncs();
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
        return -1;
    }

    it->second.lastMsg = msg;
    it->second.lastMsg.SetTo(id);
    return ret;
}

int SprMediator::NotifyAllObserver(const SprMsg& msg)
{
    for (auto& pair : mModuleMap) {
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

    // 打开模块队列时不清空：模块可能正在注册期间收发消息（如 REGISTER 前 SendMsg 的消息），
    // 清空会误删正常消息。残留清理由模块侧（队列创建者）打开时负责。
    auto pModuleMQ = make_shared<PMsgQueue>(name, MSG_MAX_SIZE, nullptr, nullptr, false);
    if (pModuleMQ->GetEvtFd() != -1) {
        result = true;
        mModuleMap[moduleId] = {monitored, {}, pModuleMQ};
        LoadMQStaticInfo(pModuleMQ->GetEvtFd(), name);
        SPR_LOGD("Register %s success! %s [%d], monitored = %d\n",
                name.c_str(), GetSprModuleIDText(moduleId).c_str(), (int)moduleId, monitored);
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
        SPR_LOGD("Unregister module %s success!\n", GetSprModuleIDText(moduleId).c_str());
    } else {
        SPR_LOGW("Not exist module %s\n", GetSprModuleIDText(moduleId).c_str());
    }

    SprMsg exitMsg(SIG_ID_PROXY_BROADCAST_EXIT_COMPONENT);
    exitMsg.SetU32Value(moduleId);
    NotifyAllObserver(exitMsg);
    return 0;
}

void SprMediator::RegisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    p->RegisterCmd(MEDIATOR_LABEL, "DumpMQDetails",     "Dump MQ details",      std::bind(&SprMediator::DebugDumpMQs, this, std::placeholders::_1));
    p->RegisterCmd(MEDIATOR_LABEL, "DumpMQWithID",      "Dump MQ with id",      std::bind(&SprMediator::DebugDumpMQWtihID, this, std::placeholders::_1));
}

void SprMediator::UnregisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    SPR_LOGD("Unregister %s all debug funcs\n", MEDIATOR_LABEL.c_str());
    p->UnregisterCmd(MEDIATOR_LABEL);
}

void SprMediator::DebugDumpMQs(const std::vector<std::string>& args)
{
    SPR_LOGI("                   Show  All Message Details (%02d)                                            \n", mModuleMap.size());
    SPR_LOGI("-----------------------------------------------------------------------------------------------\n");
    SPR_LOGI(" [ID]MODULE                          MESSAGE                                                   \n");
    SPR_LOGI("-----------------------------------------------------------------------------------------------\n");

    for (auto& pair : mModuleMap) {
        SPR_LOGI(" [%2d]%-30s  %s\n",
            pair.first,
            GetSprModuleIDText(pair.first).c_str(),
            pair.second.monitored ? pair.second.lastMsg.DumpBrief().c_str() : " - ");
    }
    SPR_LOGI("-----------------------------------------------------------------------------------------------\n");
}

void SprMediator::DebugDumpMQWtihID(const std::vector<std::string>& args)
{
    uint32_t moduleId = atoi(args[1].c_str());
    auto it = mModuleMap.find((InternalDefs::ESprModuleID)moduleId);
    if (it != mModuleMap.end()) {
        SPR_LOGI("%s from Module %s: \n", GetSigName(it->second.lastMsg.GetMsgId()), GetSprModuleIDText(moduleId).c_str());
        SPR_LOGI("%s\n", it->second.lastMsg.DumpDetails().c_str());
    }
}
