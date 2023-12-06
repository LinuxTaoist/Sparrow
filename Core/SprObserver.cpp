/**
 *------------------------------------------------------------------------------
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
 *------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *------------------------------------------------------------------------------
 *
 */
#include <iostream>
#include <string>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Util/Shared.h"
#include "SprObserver.h"
#include "SprEpollSchedule.h"

using namespace std;

#define SPR_LOGD(fmt, args...) printf("%d SprObs D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d SprObs E: " fmt, __LINE__, ##args)

const int MQ_BUFF_MAX_SIZE  = 1024;
const int RANDOM_STR_LENGTH = 8;

// Module ID, Module Name, fd,
// TODO: 初始化将自己添加到SprShcedule
SprObserver::SprObserver(ModuleIDType id, const string& name, shared_ptr<SprMediatorProxy> msgMediator)
        : mMqHandle(-1), mModuleID(id), mModuleName(name), mMsgMediatorPtr(msgMediator)
{
    MakeMQ();
    SprEpollSchedule::GetInstance()->AddPoll(*this);
    mMsgMediatorPtr->RegisterObserver(*this);
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

int SprObserver::MakeMQ()
{
    mq_attr mqAttr;
    mqAttr.mq_maxmsg = 10;      // cat /proc/sys/fs/mqueue/msg_max
    mqAttr.mq_msgsize = 1025;

    string strRandom = Shared::produceRandomStr(RANDOM_STR_LENGTH);
    mMqDevName = "/" +  mModuleName + "_" + strRandom;
    mq_unlink(mMqDevName.c_str());
    mMqHandle = mq_open(mMqDevName.c_str(), O_RDWR | O_CREAT | O_EXCL, 0666, &mqAttr);
    if(mMqHandle < 0) {
        SPR_LOGE("Open %s failed. (%s)\n", mMqDevName.c_str(), strerror(errno));
    }

    return mMqHandle;
}

// to self
int SprObserver::SendMsg(const SprMsg& msg)
{
    std::string datas;

    msg.encode(datas);
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
    int bytes = mq_receive(mMqHandle, buf, mqAttr.mq_msgsize, &prio);
    if (bytes <= 0) {
        SPR_LOGE("mq_receive failed! (%s)\n", strerror(errno));
        return -1;
    }

    string data = buf;
    return msg.decode(data);
}

int SprObserver::NotifyObserver(const SprMsg& msg)
{
    return 0;
}

int SprObserver::NotifyObserver(const vector<ModuleIDType>& ids, const SprMsg& msg)
{
    return 0;
}
