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
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "EpollEventHandler.h"

#define SPR_LOGD(fmt, args...) // printf("%4d EpollEvent D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%4d EpollEvent W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d EpollEvent E: " fmt, __LINE__, ##args)

EpollEventHandler::EpollEventHandler(int size, int blockTimeOut)
{
    if (size) {
        mHandle = epoll_create(size);
    } else {
        mHandle = epoll_create1(0);
    }

    if (mHandle < 0) {
        SPR_LOGE("epoll_create fail! (%s)\n", strerror(errno));
    }

    mRun = false;
    mTimeOut = blockTimeOut;
}

EpollEventHandler::~EpollEventHandler()
{
    ExitLoop();
}

EpollEventHandler* EpollEventHandler::GetInstance(int size, int blockTimeOut)
{
    static EpollEventHandler instance(size, blockTimeOut);
    return &instance;
}

void EpollEventHandler::AddPoll(IEpollEvent* p)
{
    //EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
    //EPOLLOUT：表示对应的文件描述符可以写；
    //EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
    struct epoll_event ep;
    ep.events = EPOLLET | EPOLLIN;
    ep.data.ptr = p;

    //EPOLL_CTL_ADD：注册新的fd到epfd中；
    //EPOLL_CTL_MOD：修改已经注册的fd的监听事件；
    //EPOLL_CTL_DEL：从epfd中删除一个fd；
    int fd = p->GetEpollFd();
    int ret = epoll_ctl(mHandle, EPOLL_CTL_ADD, fd, &ep);
    if (ret == -1) {
        SPR_LOGE("epoll_ctl fail. (%s)\n", strerror(errno));
        return ;
    }

    mEpollMap[fd] = p;
    SPR_LOGD("Add epoll fd %d\n", fd);
}

void EpollEventHandler::DelPoll(IEpollEvent* p)
{
    int ret = epoll_ctl(mHandle, EPOLL_CTL_DEL, p->GetEpollFd(), nullptr);
    if (ret != 0) {
        SPR_LOGE("epoll_ctl fail. (%s)\n", strerror(errno));
    }

    mEpollMap.erase(p->GetEpollFd());
    SPR_LOGD("Delete epoll fd %d\n", p->GetEpollFd());
}

void EpollEventHandler::HandleEpollEvent(IEpollEvent& event)
{
    event.EpollEvent(event.GetEpollFd(), event.GetEpollType(), event.GetArgs());
}

void EpollEventHandler::EpollLoop(bool bRun)
{
    struct epoll_event ep[32];
    mRun = bRun;
    while(mRun) {
        if (!mRun) {
            break;
        }

        // 无事件时, epoll_wait阻塞, 等待
        int count = epoll_wait(mHandle, ep, sizeof(ep)/sizeof(ep[0]), mTimeOut);
        if (count <= 0) {
            continue;
        }

        for (int i = 0; i < count; i++) {
            IEpollEvent* p = (IEpollEvent*)ep[i].data.ptr;
            if (p == nullptr) {
                continue;
            }

            HandleEpollEvent(*p);
        }

    }

    SPR_LOGD("EpollLoop exit\n");
}

void EpollEventHandler::ExitLoop()
{
    mRun = false;
    if (mHandle != -1)
    {
        close(mHandle);
        mHandle = -1;
    }
}