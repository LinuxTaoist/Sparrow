/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprEpollSchedule.cpp
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
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/epoll.h>
#include "SprEpollSchedule.h"

#define SPR_LOGD(fmt, args...) printf("%d EpollSch D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d EpollSch E: " fmt, __LINE__, ##args)

const uint32_t EPOLL_FD_NUM = 10;

SprEpollSchedule::SprEpollSchedule(uint32_t size)
{
    SPR_LOGD("---- Sparrow Epoll Start ----\n");

    if (size) {
        mEpollFd = epoll_create(size);
    } else {
        mEpollFd = epoll_create1(0);
    }

    if (mEpollFd == -1) {
        SPR_LOGE("epoll_create failed!\n");
    }
}

SprEpollSchedule::~SprEpollSchedule()
{
    if (mEpollFd != -1)
    {
        close(mEpollFd);
        mEpollFd = -1;
    }
}

SprEpollSchedule* SprEpollSchedule::GetInstance()
{
    static SprEpollSchedule instance(EPOLL_FD_NUM);
    return &instance;
}

void SprEpollSchedule::Init()
{
    mQuit = true;
    mpGoPool = co::AsyncCoroutinePool::Create();
    mpGoPool->InitCoroutinePool(1024);
    mpGoPool->Start(4, 128);
}

void SprEpollSchedule::Run()
{

}

void SprEpollSchedule::Exit()
{
    mQuit = false;
}

void SprEpollSchedule::AddPoll(SprObserver& observer)
{
    struct epoll_event ep;

    //EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
    //EPOLLOUT：表示对应的文件描述符可以写；
    //EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
    ep.events = EPOLLIN | EPOLLET;
    ep.data.ptr = & observer;

    //EPOLL_CTL_ADD：注册新的fd到epfd中；
    //EPOLL_CTL_MOD：修改已经注册的fd的监听事件；
    //EPOLL_CTL_DEL：从epfd中删除一个fd；
    if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, observer.getMqHandle(), &ep) != 0) {
        SPR_LOGE("%s\n", strerror(errno));
    }

    SPR_LOGD("Poll add module %s\n", observer.getModuleName().c_str());
}

void SprEpollSchedule::DelPoll(SprObserver& observer)
{
    if (epoll_ctl(mEpollFd, EPOLL_CTL_DEL, observer.getMqHandle(), nullptr) != 0) {
        SPR_LOGE("epoll_ctl failed. (%s)\n", strerror(errno));
    }

    SPR_LOGD("Poll delete module %s\n", observer.getModuleName().c_str());
}

void SprEpollSchedule::StartEpoll(bool run)
{
    struct epoll_event ep[32];

    // 触发回调处理器
    // using GoPoolCb = co::AsyncCoroutinePool::CallbackPoint;
    // std::shared_ptr<GoPoolCb> cbp(new GoPoolCb);
    // mpGoPool->AddCallbackPoint(cbp.get());

    do {
        // 无事件时, epoll_wait阻塞, -1 无限等待
        int count = epoll_wait(mEpollFd, ep, sizeof(ep)/sizeof(ep[0]), -1);
        if (count <= 0) {
            SPR_LOGE("epoll_wait failed. %s\n", strerror(errno));
            continue;
        }

        // 监听消息队列有数据, 读取数据, libgo调度
        SPR_LOGD("Data count %d come from epoll ...\n", count);
        for (int i = 0; i < count; i++) {
            SprObserver* p = static_cast<SprObserver*>(ep[i].data.ptr);
            SprMsg msg;

            // 投递任务至协程，没有回调
            mpGoPool->Post([&p, &msg] {
                if (p->RecvMsg(msg) < 0) {
                    SPR_LOGE("RecvMsg fail!\n");
                } else {
                    p->ProcessMsg(msg);
                }
            }, nullptr);
        }

    } while(!mQuit);
}
