/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprObserver.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <iostream>
#include <string>
#include <errno.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include "SprLog.h"
#include "SprObserver.h"
#include "GeneralUtils.h"
#include "CommonMacros.h"
#include "CommonTypeDefs.h"
#include "SprEpollSchedule.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGD(fmt, args...)  LOGD("SprObsBase", "[%s] " fmt, mModuleName.c_str(), ##args)
#define SPR_LOGE(fmt, args...)  LOGE("SprObsBase", "[%s] " fmt, mModuleName.c_str(), ##args)

const int MQ_BUFF_MAX_SIZE  = 1024;
const int RANDOM_STR_LENGTH = 8;

// Module ID, Module Name, proxyRpc,
SprObserver::SprObserver(ModuleIDType id, const string& name, shared_ptr<SprMediatorProxy> msgMediator, bool monitored)
{
    mConnected = false;
    mListenMQ = monitored;
    mMqHandle = -1;
    mModuleID = id;
    mModuleName = name;
    mMsgMediatorPtr = msgMediator;

    MakeMQ();
    if (IsMonitored()) {
        AddPoll(mMqHandle, IPC_TYPE_MQ);
    }

    mMsgMediatorPtr->RegisterObserver(*this);
    DumpCommonVersion();
    SPR_LOGD("Start Module: %s, mq: %s\n", mModuleName.c_str(), mMqDevName.c_str());
}

SprObserver::~SprObserver()
{
    for (const auto& it : mPollFds)
    {
        SprEpollSchedule::GetInstance()->DelPoll(it);
    }

    mMsgMediatorPtr->UnregisterObserver(*this);

    if (mMqHandle != -1)
    {
        mq_close(mMqHandle);
        mMqHandle = -1;
    }

    if (!mMqDevName.empty())
    {
        mq_unlink(mMqDevName.c_str());
        mMqDevName = "";
    }

    mPollFds.clear();
    SPR_LOGD("Exit Module: %s\n", mModuleName.c_str());
}

bool SprObserver::IsMonitored() const
{
    return mListenMQ;
}

int SprObserver::MainLoop()
{
    SprEpollSchedule::GetInstance()->EpollLoop();
    return 0;
}

int SprObserver::MainExit()
{
    SprEpollSchedule::GetInstance()->Exit();
    return 0;
}

int SprObserver::HandleEvent(int fd)
{
    return 0;
}

int SprObserver::AbstractProcessMsg(const SprMsg& msg)
{
    // SPR_LOGD("[0x%x -> 0x%x] Receive Msg: %s\n", msg.GetFrom(), msg.GetTo(), GetSigName(msg.GetMsgId()));
    switch (msg.GetMsgId())
    {
        case InternalDefs::SIG_ID_PROXY_REGISTER_RESPONSE:
            MsgResponseRegisterRsp(msg);
            break;

        case InternalDefs::SIG_ID_PROXY_UNREGISTER_RESPONSE:
            MsgResponseUnregisterRsp(msg);
            break;

        case InternalDefs::SIG_ID_SYSTEM_EXIT:
            MsgResponseSystemExitRsp(msg);
            break;

        default:
            ProcessMsg(msg);
            break;
    }

    return 0;
}

int SprObserver::AddPoll(int fd, uint8_t type)
{
    int ret = SprEpollSchedule::GetInstance()->AddPoll(fd, type, this);
    if (ret == 0)
    {
        mPollFds.insert(fd);
    }

    return ret;
}

int SprObserver::HandlePollEvent(int fd, uint8_t ipcType)
{
    SprMsg msg;
    switch (ipcType)
    {
        case IPC_TYPE_MQ:
        {
            if (RecvMsg(msg) < 0) {
                SPR_LOGE("RecvMsg fail!\n");
            } else {
                AbstractProcessMsg(msg);
            }
            break;
        }

        case IPC_TYPE_TIMERFD:
        default:
            HandleEvent(fd);
            break;
    }

    return 0;
}

// to self
int SprObserver::SendMsg(SprMsg& msg)
{
    std::string datas;

    msg.SetFrom(mModuleID);
    msg.SetTo(mModuleID);
    msg.Encode(datas);
    int ret = mq_send(mMqHandle, datas.c_str(), datas.size(), 1);
    if (ret < 0) {
        SPR_LOGE("mq_send failed! (%s)\n", strerror(errno));
    }

    return ret;
}

int SprObserver::RecvMsg(SprMsg& msg)
{
    char buf[MQ_BUFF_MAX_SIZE] = {0};
    unsigned int prio;
    mq_attr mqAttr;

    mq_getattr(mMqHandle, &mqAttr);
    int len = mq_receive(mMqHandle, buf, mqAttr.mq_msgsize, &prio);
    if (len <= 0) {
        SPR_LOGE("mq_receive failed! (%s)\n", strerror(errno));
        return -1;
    }

    string data(buf, len);
    return msg.Decode(data);
}

int SprObserver::NotifyObserver(SprMsg& msg)
{
    msg.SetFrom(mModuleID);
    mMsgMediatorPtr->NotifyObserver(msg);
    return 0;
}

int SprObserver::NotifyAllObserver(SprMsg& msg)
{
    // Notify all modules by setting destination
    // to MODULE_NONE, refer to SprMediator::NotifyAllObserver.
    msg.SetFrom(mModuleID);
    msg.SetTo(MODULE_NONE);
    mMsgMediatorPtr->NotifyAllObserver(msg);
    return 0;
}

int SprObserver::MakeMQ()
{
    mq_attr mqAttr;
    mqAttr.mq_maxmsg = 10;      // cat /proc/sys/fs/mqueue/msg_max
    mqAttr.mq_msgsize = 1025;

    string strRandom = GeneralUtils::GetRandomString(RANDOM_STR_LENGTH);
    mMqDevName = "/" +  mModuleName + "_" + strRandom;
    mq_unlink(mMqDevName.c_str());
    mMqHandle = mq_open(mMqDevName.c_str(), O_RDWR | O_CREAT | O_EXCL, 0666, &mqAttr);
    if(mMqHandle < 0) {
        SPR_LOGE("Open %s failed. (%s)\n", mMqDevName.c_str(), strerror(errno));
    }

    return mMqHandle;
}

int SprObserver::DumpCommonVersion()
{
    SPR_LOGD("- Dump common version: %s / %s / %s \n",
                COMMON_TYPE_DEFS_VERSION, COMMON_MACROS_VERSION, CORE_TYPE_DEFS_VERSION);
    return 0;
}

int SprObserver::MsgResponseSystemExitRsp(const SprMsg& msg)
{
    SPR_LOGD("System Exit!\n");
    MainExit();
    return 0;
}

int SprObserver::MsgResponseRegisterRsp(const SprMsg& msg)
{
    SPR_LOGD("Register Successfully!\n");
    mConnected = msg.GetU8Value();
    return 0;
}

int SprObserver::MsgResponseUnregisterRsp(const SprMsg& msg)
{
    // 注销成功，连接状态为false
    SPR_LOGD("Unregister Successfully!\n");
    mConnected = !msg.GetU8Value();
    return 0;
}
