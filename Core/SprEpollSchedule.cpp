/**
 *---------------------------------------------------------------------------------------------------------------------
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
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/epoll.h>
#include "LibgoAdapter.h"
#include "DefineMacro.h"
#include "SprEpollSchedule.h"

#define SPR_LOGD(fmt, args...) LOGD("EpollSch", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("EpollSch", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("EpollSch", fmt, ##args)

const uint32_t EPOLL_FD_NUM = 10;

SprEpollSchedule::SprEpollSchedule(uint32_t size)
{
    SPR_LOGD("===========  Sparrow Epoll Start  ===========\n");

    if (size) {
        mEpollHandle = epoll_create(size);
    } else {
        mEpollHandle = epoll_create1(0);
    }

    if (mEpollHandle == -1) {
        SPR_LOGE("epoll_create fail! (%s)\n", strerror(errno));
    }

    mRun = true;
    Init();
}

SprEpollSchedule::~SprEpollSchedule()
{
    if (mEpollHandle != -1)
    {
        close(mEpollHandle);
        mEpollHandle = -1;
    }
}

SprEpollSchedule* SprEpollSchedule::GetInstance()
{
    static SprEpollSchedule instance(EPOLL_FD_NUM);
    return &instance;
}

void SprEpollSchedule::Init()
{
    mCoPool.InitCoroutinePool(1024);
    mCoPool.Start(10, 128);
}

void SprEpollSchedule::Exit()
{
    mRun = false;
    if (mEpollHandle != -1) {
        close(mEpollHandle);
        mEpollHandle = -1;
    }
}

int SprEpollSchedule::AddPoll(int fd, uint8_t ipcType, SprObserver* observer)
{
    //EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
    //EPOLLOUT：表示对应的文件描述符可以写；
    //EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
    struct epoll_event ep;
    ep.events = EPOLLIN | EPOLLET;
    ep.data.fd = fd;

    //EPOLL_CTL_ADD：注册新的fd到epfd中；
    //EPOLL_CTL_MOD：修改已经注册的fd的监听事件；
    //EPOLL_CTL_DEL：从epfd中删除一个fd；
    int ret = epoll_ctl(mEpollHandle, EPOLL_CTL_ADD, fd, &ep);
    if (ret != 0) {
        SPR_LOGE("epoll_ctl fail. (%s)\n", strerror(errno));
    } else {
        mPollMap.insert(std::make_pair(fd, std::make_pair(ipcType, observer)));
        SPR_LOGD("Poll add module %s\n", observer->GetModuleName().c_str());
    }

    return ret;
}

void SprEpollSchedule::DelPoll(int fd)
{
    if (epoll_ctl(mEpollHandle, EPOLL_CTL_DEL, fd, nullptr) != 0) {
        SPR_LOGE("epoll_ctl fail. (%s)\n", strerror(errno));
    }
}

void SprEpollSchedule::EpollLoop()
{
    struct epoll_event ep[32];

    // 触发回调处理器
    // using GoPoolCb = co::AsyncCoroutinePool::CallbackPoint;
    // std::shared_ptr<GoPoolCb> cbp(new GoPoolCb);
    // mCoPool.AddCallbackPoint(cbp.get());

    do {
        // 无事件时, epoll_wait阻塞, 超时等待
        int count = epoll_wait(mEpollHandle, ep, sizeof(ep)/sizeof(ep[0]), 5000);
        if (count <= 0) {
            if (!mRun) {
                break;
            }

            continue;
        }

        // IO监听有数据, libgo调度
        for (int i = 0; i < count; i++) {
            int fd = ep[i].data.fd;

            // SPR_LOGD("Data count %d come from epoll %d\n", count, fd);
            if (mPollMap.count(fd) != 0 && mPollMap[fd].second != nullptr)
            {
                // 投递任务至协程，没有回调
                mCoPool.Post([&] {
                    int ipcType = mPollMap[fd].first;
                    // SPR_LOGD("HandlePollEvent fd = %d, ipc = %d\n", fd, ipcType);
                    mPollMap[fd].second->HandlePollEvent(fd, ipcType);
                }, nullptr);
            } else {
                SPR_LOGE("fd %d not found\n", fd);
            }


        }
    } while(mRun);
}
