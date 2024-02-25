/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprObserver.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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
#include "Util/Shared.h"
#include "SprObserver.h"
#include "SprEpollSchedule.h"

using namespace std;
using namespace InternalEnum;

#define SPR_LOGD(fmt, args...) printf("%d SprObs D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d SprObs E: " fmt, __LINE__, ##args)

const int MQ_BUFF_MAX_SIZE  = 1024;
const int RANDOM_STR_LENGTH = 8;

// Module ID, Module Name, proxyRpc,
SprObserver::SprObserver(ModuleIDType id, const string& name, shared_ptr<SprMediatorProxy> msgMediator)
{
    mConnected = false;
    mMqHandle = -1;
    mCurListenHandler = -1;
    mCurListenEventType = POLL_SCHEDULE_TYPE_MQ;
    mModuleID = id;
    mModuleName = name;
    mMsgMediatorPtr = msgMediator;

    MakeMQ();
    if (IsListenMQ()) {
        AddPoll(POLL_SCHEDULE_TYPE_MQ, mMqHandle);
    }

    mMsgMediatorPtr->RegisterObserver(*this);

    InitSigHandler();
    SPR_LOGD("Start Module: %s, mq: %s\n", mModuleName.c_str(), mMqDevName.c_str());
}

SprObserver::~SprObserver()
{
    SprEpollSchedule::GetInstance()->DelPoll(*this);
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

    SPR_LOGD("Exit Module: %s\n", mModuleName.c_str());
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

static void SignalHandler(int signum)
{
    SPR_LOGD("signal handler received signal %d!\n", signum);

    switch (signum)
    {
        case SIGTERM:
        case SIGINT:
        {
            SprObserver::MainExit();
            break;
        }

        default:
            break;
    }
}

void SprObserver::InitSigHandler()
{
    struct sigaction signal_action;

    signal_action.sa_handler = SignalHandler;
    signal_action.sa_flags = 0;
    sigemptyset(&signal_action.sa_mask);

    /* register signals */
    sigaction(SIGTERM, &signal_action, NULL);
    sigaction(SIGINT, &signal_action, NULL);
    sigaction(SIGPIPE, &signal_action, NULL); /* ignore broken pipe signals */
    sigaction(SIGABRT, &signal_action, NULL);
}

int SprObserver::AbstractProcessMsg(const SprMsg& msg)
{
    // SPR_LOGD("[0x%x -> 0x%x] Receive Msg: %s\n", msg.GetFrom(), msg.GetTo(), GetSigName(msg.GetMsgId()));
    switch (msg.GetMsgId())
    {
        case InternalEnum::SIG_ID_PROXY_REGISTER_RESPONSE:
            MsgResponseRegisterRsp(msg);
            break;

        case InternalEnum::SIG_ID_PROXY_UNREGISTER_RESPONSE:
            MsgResponseUnregisterRsp(msg);
            break;

        case InternalEnum::SIG_ID_SYSTEM_EXIT:
            MsgResponseSystemExitRsp(msg);
            break;

        default:
            ProcessMsg(msg);
            break;
    }

    return 0;
}

int SprObserver::AddPoll(uint32_t listenType, int listenHandler)
{
    SetCurListenEventType(listenType);
    SetCurListenHandler(listenHandler);
    SprEpollSchedule::GetInstance()->AddPoll(*this);
    return 0;
}

int SprObserver::HandlePollEvent()
{
    SprMsg msg;
    switch (GetCurListenEventType())
    {
        case POLL_SCHEDULE_TYPE_MQ:
        {
            if (RecvMsg(msg) < 0) {
                SPR_LOGE("RecvMsg fail!\n");
            } else {
                AbstractProcessMsg(msg);
            }
            break;
        }

        case POLL_SCHEDULE_TYPE_TIMER:
        default:
            ProcessMsg(msg);
            break;
    }

    return 0;
}

// to self
int SprObserver::SendMsg(const SprMsg& msg)
{
    std::string datas;

    msg.Encode(datas);
    int ret = mq_send(mMqHandle, (const char*)datas.c_str(), datas.size(), 1);
    if (ret < 0) {
        SPR_LOGE("mq_send failed! (%s)\n", strerror(errno));
    }

    return ret;
}

// from self
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

int SprObserver::NotifyObserver(const SprMsg& msg)
{
    mMsgMediatorPtr->NotifyObserver(msg);
    return 0;
}

int SprObserver::NotifyAllObserver(const SprMsg& msg)
{
    mMsgMediatorPtr->NotifyAllObserver(msg);
    return 0;
}

int SprObserver::MakeMQ()
{
    mq_attr mqAttr;
    mqAttr.mq_maxmsg = 10;      // cat /proc/sys/fs/mqueue/msg_max
    mqAttr.mq_msgsize = 1025;

    string strRandom = Shared::ProduceRandomStr(RANDOM_STR_LENGTH);
    mMqDevName = "/" +  mModuleName + "_" + strRandom;
    mq_unlink(mMqDevName.c_str());
    mMqHandle = mq_open(mMqDevName.c_str(), O_RDWR | O_CREAT | O_EXCL, 0666, &mqAttr);
    if(mMqHandle < 0) {
        SPR_LOGE("Open %s failed. (%s)\n", mMqDevName.c_str(), strerror(errno));
    }

    return mMqHandle;
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
