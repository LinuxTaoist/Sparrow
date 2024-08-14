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
#include <stdio.h>
#include <errno.h>
#include <mqueue.h>
#include <string.h>
#include <sys/resource.h>
#include "PMsgQueue.h"
#include "EpollEventHandler.h"

bool PMsgQueue::mUnlimit = false;

#define SPR_LOGD(fmt, args...) printf("%4d PMsgQueue D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d PMsgQueue E: " fmt, __LINE__, ##args)

PMsgQueue::PMsgQueue(const std::string& name, long maxmsg,
            std::function<void(int, std::string, void* arg)>cb,
            void* arg)
    : IEpollEvent(-1, EPOLL_TYPE_MQUEUE, arg), mCb(cb)
{
    mName = name;
    mMaxMsg = maxmsg;

    if (!mUnlimit)
    {
        struct rlimit rlim = {RLIM_INFINITY, RLIM_INFINITY};
        int ret = getrlimit(RLIMIT_MSGQUEUE, &rlim);
        if (ret == 0)
        {
            rlim.rlim_cur = RLIM_INFINITY;      // soft limit
            rlim.rlim_max = RLIM_INFINITY;      // hard limit
            setrlimit(RLIMIT_MSGQUEUE, &rlim);
        }

        mUnlimit = true;
    }

    OpenMsgQueue();
}

PMsgQueue::~PMsgQueue()
{
}

void PMsgQueue::OpenMsgQueue()
{
    struct mq_attr mqAttr;
    mqAttr.mq_maxmsg = 10;
    mqAttr.mq_msgsize = 1025;
    mqAttr.mq_flags = 0;
    mqAttr.mq_curmsgs = 0;
    mqAttr.__pad[0] = 0;
    mqAttr.__pad[1] = 0;
    mqAttr.__pad[2] = 0;
    mqAttr.__pad[3] = 0;
    mEpollFd = (int)mq_open(mName.c_str(), O_CREAT | O_RDWR | O_NONBLOCK, 0, &mqAttr);
    if (mEpollFd < 0) {
        SPR_LOGE("mq_open failed! (%s)\n", strerror(errno));
        return;
    }
}

int32_t PMsgQueue::Clear()
{
    uint32_t prio = 0;
    uint32_t msgCnt = 0;
    std::string msg;

    while (Recv(msg, prio) > 0) {
        SPR_LOGD("message read from queue<%s> cnt = %u\n", mName.c_str(), ++msgCnt);
    }

    return 0;
}

int32_t PMsgQueue::Send(const std::string& msg, uint32_t prio)
{
    int ret = mq_send(mEpollFd, msg.c_str(), msg.size(), prio);
    if (ret < 0) {
        SPR_LOGE("mq_send failed! (%s)\n", strerror(errno));
    }

    return 0;
}

int32_t PMsgQueue::Recv(std::string& msg, uint32_t& prio)
{
    mq_attr mqAttr;
    mq_getattr(mEpollFd, &mqAttr);

    char buf[mqAttr.mq_msgsize];
    int len = mq_receive(mEpollFd, buf, mqAttr.mq_msgsize, &prio);
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
        mCb(fd, msg, arg);
    }

    return nullptr;
}
