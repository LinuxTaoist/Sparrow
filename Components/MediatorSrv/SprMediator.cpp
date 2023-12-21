/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediator.cpp
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
#include <string>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <sys/epoll.h>
#include <mqueue.h>
#include <string.h>
#include "SprCommonType.h"
#include "SprMediator.h"

using namespace std;
using namespace InternalEnum;

#define SPR_LOGD(fmt, args...) printf("%d SprMediator D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%d SprMediator W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d SprMediator E: " fmt, __LINE__, ##args)

const uint32_t EPOLL_FD_NUM = 10;

SprMediator::SprMediator(int size)
{
    if (size) {
        mEpollHandler = epoll_create(size);
    } else {
        mEpollHandler = epoll_create1(0);
    }
}

SprMediator::~SprMediator()
{
    DestroyInternalPort();

    for (auto& pair : mModuleMap)
    {
        if (pair.second.handler != -1)
        {
            mq_close(pair.second.handler);
            pair.second.handler = -1;
        }
    }

    if (mEpollHandler != -1)
    {
        close(mEpollHandler);
        mEpollHandler = -1;
    }

    mModuleMap.clear();
}

SprMediator* SprMediator::GetInstance()
{
    static SprMediator instance(EPOLL_FD_NUM);
    return &instance;
}

int SprMediator::Init()
{
    if (mEpollHandler == -1) {
        SPR_LOGE("epoll_create failed! (%s)\n", strerror(errno));
        return -1;
    }

    SPR_LOGD("--- Start proxy server ---\n");
    PrepareInternalPort();
    StartEpoll();

    return 0;
}

int SprMediator::MakeMQ(string name)
{
    mq_attr mqAttr;
    mqAttr.mq_maxmsg = 10;      // cat /proc/sys/fs/mqueue/msg_max
    mqAttr.mq_msgsize = 1025;

    int handler = mq_open(name.c_str(), O_RDWR | O_CREAT, 0666, &mqAttr);
    if(handler < 0) {
        SPR_LOGE("Open %s failed. (%s)\n", name.c_str(), strerror(errno));
    }

    return handler;
}

int SprMediator::MakeUnixDgramSocket(std::string ip, uint16_t port)
{
    return 0;
}

int SprMediator::PrepareInternalPort()
{
    mq_unlink(MEDIATOR_MSG_QUEUE);
    mHandler = MakeMQ(MEDIATOR_MSG_QUEUE);

    struct epoll_event ep;
    ep.events = EPOLLIN | EPOLLET;
    ep.data.fd = mHandler;
    if (epoll_ctl(mEpollHandler, EPOLL_CTL_ADD, mHandler, &ep) != 0) {
        SPR_LOGE("epoll_ctl fail! (%s)\n", strerror(errno));
    }

    SPR_LOGD("Open Internal Port: %s.\n", MEDIATOR_MSG_QUEUE);
    return 0;
}

int SprMediator::DestroyInternalPort()
{
    if (mHandler != -1)
    {
        mq_close(mHandler);
        mHandler = -1;
    }

    return 0;
}

int SprMediator::StartEpoll()
{
    struct epoll_event ep[32];

    do {
        // 无事件时, epoll_wait阻塞, -1 无限等待
        int count = epoll_wait(mEpollHandler, ep, sizeof(ep)/sizeof(ep[0]), -1);
        if (count <= 0) {
            SPR_LOGE("epoll_wait failed. %s\n", strerror(errno));
            continue;
        }

        for (int i = 0; i < count; i++) {
            int handler = ep[i].data.fd;
            char buf[MSG_BUF_MAX_LENGTH] = {0};
            unsigned int prio;
            mq_attr mqAttr;

            mq_getattr(handler, &mqAttr);
            int len = mq_receive(handler, buf, mqAttr.mq_msgsize, &prio);
            if (len <= 0) {
                SPR_LOGE("mq_receive failed! (%s)\n", strerror(errno));
                return -1;
            }

            string datas(buf, len);
            SprMsg msg(datas);
            ProcessMsg(msg);
        }
    } while(1);

    return 0;
}

int SprMediator::ProcessMsg(const SprMsg& msg)
{
    SPR_LOGD("Receive msg: 0x%x\n", msg.GetMsgId());

    switch (msg.GetMsgId())
    {
        case PROXY_MSG_REGISTER_REQUEST:
            MsgResponseRegister(msg);
            break;

        case PROXY_MSG_UNREGISTER_REQUEST:
            MsgResponseUnregister(msg);
            break;

        default:
            NotifyAllObserver(msg);
            break;
    }

    return 0;
}

int SprMediator::SendMsg(const SprMsg& msg)
{
    string datas;

    msg.Encode(datas);
    int ret = mq_send(mHandler, (const char*)datas.c_str(), datas.size(), 1);
    if (ret < 0) {
        SPR_LOGE("mq_send failed! (%s)\n", strerror(errno));
    }

    return 0;
}

int SprMediator::NotifyObserver(ESprModuleID id, const SprMsg& msg)
{
    auto it = mModuleMap.find(id);
    if (it == mModuleMap.end())
    {
        SPR_LOGW("Not exist moduleId: 0x%x\n", id);
        return 0;
    }

    string datas;
    msg.Encode(datas);
    int ret = mq_send(it->second.handler, (const char*)datas.c_str(), datas.size(), 1);
    if (ret < 0) {
        SPR_LOGE("mq_send failed! (%s)\n", strerror(errno));
    }

    return ret;
}

int SprMediator::NotifyAllObserver(const SprMsg& msg)
{
    for (const auto& pair : mModuleMap)
    {
        if (pair.second.handler == -1)
        {
            continue;
        }

        // SPR_LOGD("Module Id = 0x%x\n", pair.first);
        NotifyObserver(pair.first, msg);
    }

    return 0;
}

int SprMediator::MsgResponseRegister(const SprMsg& msg)
{
    bool result = false;
    // EProxyType type = static_cast<EProxyType>(msg.GetU32Value());
    ESprModuleID moduleId = static_cast<ESprModuleID>(msg.GetU16Value());
    std::string name = msg.GetString();

    auto it = mModuleMap.find(moduleId);
    if (it != mModuleMap.end())
    {
        SPR_LOGW("Already exist moduleId: 0x%x\n", moduleId);
        if (it->second.handler != -1)
        {
            mq_close(it->second.handler);
            it->second.handler = -1;
        }
        mModuleMap.erase(moduleId);
    }

    int handler = MakeMQ(name);
    if (handler != -1)
    {
        result = true;
        mModuleMap[moduleId] = { handler, name };
        SPR_LOGD("Register successfully! ID: %d, NAME: %s\n", (int)moduleId, name.c_str());
    } else {
        SPR_LOGE("Invaild handler!\n");
    }

    SprMsg rspMsg(PROXY_MSG_REGISTER_RESPONSE);
    rspMsg.SetU8Value(result);
    NotifyObserver(moduleId, rspMsg);

    return 0;
}

int SprMediator::MsgResponseUnregister(const SprMsg& msg)
{
    bool result = false;

    // EProxyType type = static_cast<EProxyType>(msg.GetU32Value());
    ESprModuleID moduleId = static_cast<ESprModuleID>(msg.GetU16Value());
    std::string name = msg.GetString();

    auto it = mModuleMap.find(moduleId);
    if (it != mModuleMap.end())
    {
        SPR_LOGW("Not exist module id: %x\n", moduleId);

        if (it->second.handler != -1)
        {
            mq_close(it->second.handler);
            it->second.handler = -1;
            SPR_LOGD("Close mq %s \n", it->second.name.c_str());
            result = true;
        }

        mModuleMap.erase(moduleId);
    }

    SprMsg rspMsg(PROXY_MSG_UNREGISTER_RESPONSE);
    rspMsg.SetU8Value(result);
    NotifyObserver(moduleId, rspMsg);
    SPR_LOGD("Unregister successfully! ID: %d, NAME: %s\n", (int)moduleId, name.c_str());

    return 0;
}
