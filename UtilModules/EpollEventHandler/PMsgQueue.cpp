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
#include <stdio.h>
#include <errno.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include "PMsgQueue.h"
#include "EpollEventHandler.h"

#define SPR_LOGD(fmt, args...) // printf("%6d PMsgQueue D: %4d [%s] " fmt, getpid(), __LINE__, mDevName.c_str(), ##args)
#define SPR_LOGW(fmt, args...) // printf("%6d PMsgQueue W: %4d [%s] " fmt, getpid(), __LINE__, mDevName.c_str(), ##args)
#define SPR_LOGE(fmt, args...) printf("%6d PMsgQueue E: %4d [%s] " fmt, getpid(), __LINE__, mDevName.c_str(), ##args)

PMsgQueue::PMsgQueue(const std::string& name, long maxmsg,
            std::function<void(int, std::string, void*)>cb,
            void* arg)
    : IEpollEvent(-1, EPOLL_TYPE_MQUEUE, arg), mMaxMsg(maxmsg), mCb(cb)
{
    if (name.empty()) {
        SetReady(false);
        return;
    }

    mDevName = (name[0] == '/') ? name : "/" + name;
    mMaxMsg = maxmsg;
    int rc = InitMsgQueue();
    if (rc == -1) {
        SetReady(false);
    }
}

PMsgQueue::~PMsgQueue()
{
    Close();
    if (!mDevName.empty()) {
        mq_unlink(mDevName.c_str());
    }
}

int PMsgQueue::InitMsgQueue()
{
    if (mDevName.empty()) {
        SPR_LOGE("mDevName is empty!\n");
        return -1;
    }

    bool isExist = true;
    mEvtFd = mq_open(mDevName.c_str(), O_RDWR | O_NONBLOCK);
    if (mEvtFd == (mqd_t)-1) {
        isExist = false;
        if (errno == ENOENT) {
            struct mq_attr mqAttr;   // cat /proc/sys/fs/mqueue/msg_max
            mqAttr.mq_maxmsg = 10;
            mqAttr.mq_msgsize = 1025;
            mqAttr.mq_flags = 0;
            mqAttr.mq_curmsgs = 0;
            mqAttr.__pad[0] = 0;
            mqAttr.__pad[1] = 0;
            mqAttr.__pad[2] = 0;
            mqAttr.__pad[3] = 0;
            mEvtFd = (int)mq_open(mDevName.c_str(), O_RDWR | O_NONBLOCK | O_CREAT | O_EXCL, 0666, &mqAttr);
            if (mEvtFd == (mqd_t)-1) {
                SPR_LOGE("mq_open %s failed! (%s)\n", mDevName.c_str(), strerror(errno));
                return -1;
            }
        }
    }

    if (isExist) {
        std::string msg;
        ssize_t received = 0;
        uint32_t prio = 0;
        while ((received = Recv(msg, prio)) > 0) {
            SPR_LOGD("clear message from queue<%s> cnt = %ld\n", mDevName.c_str(), received);
        }
    }

    return 0;
}

int32_t PMsgQueue::Clear()
{
    std::string msg;
    uint32_t prio = 0;

    while (Recv(msg, prio) > 0) {
        SPR_LOGD("Clear queue %s, cnt = %d\n", mDevName.c_str());
    }

    return 0;
}

int32_t PMsgQueue::Send(int fd, const char* data, size_t size, uint32_t prio)
{
    int32_t ret = mq_send(fd, data, size, prio);
    if (ret < 0) {
        SPR_LOGE("mq_send failed! (%s)\n", strerror(errno));
        return -1;
    }

    return ret;
}

int32_t PMsgQueue::Send(const std::string& msg, uint32_t prio)
{
    return Send(mEvtFd, msg.c_str(), msg.size(), prio);
}

int32_t PMsgQueue::Recv(int fd, char* data, size_t size, uint32_t& prio)
{
    if (!data || size == 0) {
        return -1;
    }

    int32_t len = mq_receive(fd, data, size, &prio);
    if (len <= 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            SPR_LOGW("mq_receive EAGAIN (%s)\n", strerror(errno));
            return 0;
        } else {
            SPR_LOGE("mq_receive failed! (%s)\n", strerror(errno));
            return -1;
        }
    }

    return len;
}
int32_t PMsgQueue::Recv(std::string& msg, uint32_t& prio)
{
    mq_attr mqAttr;
    mq_getattr(mEvtFd, &mqAttr);
    char buf[mqAttr.mq_msgsize] = {0};

    int32_t len = Recv(mEvtFd, buf, sizeof(buf), prio);
    if (len > 0) {
        msg.assign(buf, len);
    }
    return len;
}

void PMsgQueue::Close()
{
    if (mEvtFd == -1) {
        return;
    }

    SPR_LOGD("Close fd: %d\n", mEvtFd);
    DelFromPoll();
    mq_close(mEvtFd);
    mEvtFd = -1;
}

ssize_t PMsgQueue::Write(int fd, const char* data, size_t size)
{
    return Send(fd, data, size);
}

ssize_t PMsgQueue::Read(int fd, char* data, size_t size)
{
    uint32_t prio = 0;
    return Recv(fd, data, size, prio);
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
