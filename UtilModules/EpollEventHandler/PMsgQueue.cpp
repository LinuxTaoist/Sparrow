/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PMsgQueue.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/14
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/08/14 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <iostream>
#include <random>
#include <stdio.h>
#include <errno.h>
#include <mqueue.h>
#include <string.h>
#include <sys/resource.h>
#include "PMsgQueue.h"
#include "EpollEventHandler.h"

#define SPR_LOGD(fmt, args...) printf("%4d PMsgQueue D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d PMsgQueue E: " fmt, __LINE__, ##args)

PMsgQueue::PMsgQueue(const std::string& name, long maxmsg,
            std::function<void(int, std::string, void*)>cb,
            void* arg)
    : IEpollEvent(-1, EPOLL_TYPE_MQUEUE, arg), mMaxMsg(maxmsg), mCb(cb)
{
    mDevName = "/" + name + "_" + GetRandomString(8);
    mMaxMsg = maxmsg;
    OpenMsgQueue();
}

PMsgQueue::~PMsgQueue()
{
    if (!mDevName.empty() && mEpollFd > 0) {
        Clear();
        mq_close(mEpollFd);
        mq_unlink(mDevName.c_str());
        mDevName = "";
    }
}

std::string PMsgQueue::GetRandomString(int32_t width)
{
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, sizeof(alphanum) - 2);

    std::string str(width, 0);
    for (auto &c : str) {
        c = alphanum[dist(rng)];
    }

    return str;
}

void PMsgQueue::OpenMsgQueue()
{
    if (mDevName.empty()) {
        SPR_LOGE("mDevName is empty!\n");
        return;
    }

    mq_unlink(mDevName.c_str());
    struct mq_attr mqAttr;   // cat /proc/sys/fs/mqueue/msg_max
    mqAttr.mq_maxmsg = 10;
    mqAttr.mq_msgsize = 1025;
    mqAttr.mq_flags = 0;
    mqAttr.mq_curmsgs = 0;
    mqAttr.__pad[0] = 0;
    mqAttr.__pad[1] = 0;
    mqAttr.__pad[2] = 0;
    mqAttr.__pad[3] = 0;
    mEpollFd = (int)mq_open(mDevName.c_str(), O_CREAT | O_RDWR | O_NONBLOCK | O_EXCL, 0666, &mqAttr);
    if (mEpollFd < 0) {
        SPR_LOGE("mq_open %s failed! (%s)\n", mDevName.c_str(), strerror(errno));
        return;
    }
}

int32_t PMsgQueue::Clear()
{
    uint32_t prio = 0;
    uint32_t msgCnt = 0;
    std::string msg;

    while (Recv(msg, prio) > 0) {
        SPR_LOGD("clear message from queue<%s> cnt = %u\n", mDevName.c_str(), ++msgCnt);
    }

    return 0;
}

int32_t PMsgQueue::Send(const std::string& msg, uint32_t prio)
{
    int32_t ret = mq_send(mEpollFd, msg.c_str(), msg.size(), prio);
    if (ret < 0) {
        SPR_LOGE("mq_send failed! (%s)\n", strerror(errno));
        return -1;
    }

    return 0;
}

int32_t PMsgQueue::Recv(std::string& msg, uint32_t& prio)
{
    mq_attr mqAttr;
    mq_getattr(mEpollFd, &mqAttr);

    char buf[1025] = {0};
    int32_t len = mq_receive(mEpollFd, buf, mqAttr.mq_msgsize, &prio);
    if (len <= 0) {
        SPR_LOGE("mq_receive failed! (%s)\n", strerror(errno));
        return -1;
    }

    msg.assign(buf, len);
    return 0;
}

void* PMsgQueue::EpollEvent(int fd, EpollType eType, void* arg)
{
    std::string msg;
    uint32_t prio = 0;
    if (Recv(msg, prio) < 0) {
        return nullptr;
    }

    if (mCb) {
        arg = arg ? arg : this;
        mCb(fd, msg, arg);
    }

    return nullptr;
}
