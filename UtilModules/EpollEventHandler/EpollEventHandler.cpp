/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : EpollEventHandler.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/05/07
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/05/07 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <atomic>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "PLog.h"
#include "EpollEventHandler.h"

#define PLOG_TAG "EpEvtHandler"

static std::atomic<bool> gObjAlive(true);

EpollEventHandler::EpollEventHandler(int32_t size, int32_t blockTimeOut)
{
    if (size) {
        mHandle = epoll_create(size);
    } else {
        mHandle = epoll_create1(0);
    }

    if (mHandle < 0) {
        PLOGE("epoll_create fail! (%s)\n", strerror(errno));
    }

    mRun = false;
    mTimeOut = blockTimeOut;
}

EpollEventHandler::~EpollEventHandler()
{
    gObjAlive = false;
    ExitLoop();
}

EpollEventHandler* EpollEventHandler::GetInstance(int32_t size, int32_t blockTimeOut)
{
    if (!gObjAlive) {
        return nullptr;
    }

    static EpollEventHandler instance(size, blockTimeOut);
    return &instance;
}

void EpollEventHandler::AddPoll(IEpollEvent* p)
{
    //EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
    //EPOLLOUT：表示对应的文件描述符可以写；
    //EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的，默认水平触发。
    struct epoll_event ep;
    ep.events = EPOLLIN;
    ep.data.ptr = p;

    //EPOLL_CTL_ADD：注册新的fd到epfd中；
    //EPOLL_CTL_MOD：修改已经注册的fd的监听事件；
    //EPOLL_CTL_DEL：从epfd中删除一个fd；
    int32_t fd = p->GetEvtFd();
    if (fd < 0) {
        PLOGE("Invalid fd: %d\n", fd);
        return;
    }

    // Detect duplicate registration (warn but allow overwrite)
    if (mEpollMap.find(fd) != mEpollMap.end()) {
        PLOGW("fd %d already in poll map, replacing\n", fd);
    }

    int32_t ret = epoll_ctl(mHandle, EPOLL_CTL_ADD, fd, &ep);
    if (ret == -1) {
        PLOGE("epoll_ctl %d fail. (%s)\n", fd, strerror(errno));
        return ;
    }

    mEpollMap[fd] = p;
    PLOGD("Add epoll fd %d\n", fd);
}

void EpollEventHandler::DelPoll(IEpollEvent* p)
{
    if (p == nullptr) {
        PLOGE("p is null\n");
        return ;
    }

    int32_t ret = epoll_ctl(mHandle, EPOLL_CTL_DEL, p->GetEvtFd(), nullptr);
    if (ret != 0) {
        PLOGE("epoll_ctl %d fail. (%s)\n", p->GetEvtFd(), strerror(errno));
    }

    mEpollMap.erase(p->GetEvtFd());
    PLOGD("Delete epoll fd %d\n", p->GetEvtFd());
}

void EpollEventHandler::HandleEpollEvent(IEpollEvent& event)
{
    event.EpollEvent(event.GetEvtFd(), event.GetEpollType(), event.GetArgs());
}

void EpollEventHandler::EpollLoop()
{
    struct epoll_event ep[32];
    const int32_t maxEvents = static_cast<int32_t>(sizeof(ep)/sizeof(ep[0]));
    mRun = true;
    while(mRun) {
        // 无事件时, epoll_wait阻塞, 等待
        int32_t count = epoll_wait(mHandle, ep, maxEvents, mTimeOut);
        if (count <= 0) {
            if (count < 0 && errno != EINTR) {
                PLOGE("epoll_wait fail! (%s)\n", strerror(errno));
            }
            continue;
        }

        // Warn if event buffer is full (possible event loss)
        if (count >= maxEvents) {
            PLOGW("epoll_wait returned max %d events (possible overflow)\n", count);
        }

        for (int32_t i = 0; i < count; i++) {
            IEpollEvent* p = reinterpret_cast<IEpollEvent*>(ep[i].data.ptr);
            if (p == nullptr) {
                continue;
            }

            HandleEpollEvent(*p);
        }
    }

    PLOGD("EpollLoop exit\n");
}

void EpollEventHandler::ExitLoop()
{
    mRun = false;
    if (mHandle != -1) {
        close(mHandle);
        mHandle = -1;
    }
}
