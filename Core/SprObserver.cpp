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
#include <random>
#include <chrono>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SprObserver.h"

using namespace std;

#define SPR_LOGD(fmt, args...) printf("%d SprObs D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d SprObs E: " fmt, __LINE__, ##args)

const int MQ_BUFF_MAX_SIZE  = 1024;
const int RANDOM_STR_LENGTH = 8;

// Module ID, Module Name, fd,
SprObserver::SprObserver(ModuleIDType id, const string& name, shared_ptr<SprMediatorProxy> msgMediator)
        : mMqHandle(-1), mModuleID(id), mModuleName(name), mMsgMediatorPtr(msgMediator)
{
    mkMq();
    mMsgMediatorPtr->RegisterObserver(*this);
    SPR_LOGD("Start Module: %s, mq: %s\n", mModuleName.c_str(), mMqDevName.c_str());
}

SprObserver::~SprObserver()
{
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
}

std::string generateRandomString(const int len)
{
    std::string strRandom;
    const std::string seedStr = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(0, seedStr.size() - 1);

    for (int i = 0; i < len; i++) {
        int randomValue = distribution(generator);
        strRandom += seedStr[randomValue];
    }

    return strRandom;
}

int SprObserver::mkMq()
{
    mq_attr mqAttr;
    mqAttr.mq_maxmsg = 10;      // cat /proc/sys/fs/mqueue/msg_max
    mqAttr.mq_msgsize = 1025;

    string strRandom = generateRandomString(RANDOM_STR_LENGTH);
    mMqDevName = "/" +  mModuleName + "_" + strRandom;
    mq_unlink(mMqDevName.c_str());
    mMqHandle = mq_open(mMqDevName.c_str(), O_RDWR | O_CREAT | O_EXCL, 0666, &mqAttr);
    if(mMqHandle < 0) {
        SPR_LOGE("Open %s failed. (%s)\n", mMqDevName.c_str(), strerror(errno));
    }

    return 0;
}

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

    return msg.decode(buf, bytes);
}

int SprObserver::NotifyObserver(const SprMsg& msg)
{
    return 0;
}

int SprObserver::NotifyObserver(const vector<ModuleIDType>& ids, const SprMsg& msg)
{
    return 0;
}
