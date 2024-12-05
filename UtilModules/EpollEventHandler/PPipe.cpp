/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PPipe.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/10/10
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/10/10 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "PPipe.h"
#include "EpollEventHandler.h"

#define SPR_LOGD(fmt, args...) printf("%4d PPipe D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d PPipe E: " fmt, __LINE__, ##args)

PPipe::PPipe(int fd, std::function<void(int, std::string, void*)> cb, void *arg)
    : IEpollEvent(fd, EPOLL_TYPE_PIPE, arg), mFifoFd(-1), mCb(cb)
{
    int flags = fcntl(mEvtFd, F_GETFL, 0);
    fcntl(mEvtFd, F_SETFL, flags | O_NONBLOCK);
}

PPipe::PPipe(const std::string& fileName, std::function<void(int, std::string, void*)> cb, void* arg)
    : IEpollEvent(-1, EPOLL_TYPE_PIPE, arg), mCb(cb)
{
    unlink(fileName.c_str());
    if (mkfifo(fileName.c_str(), 0666) == -1) {
        SPR_LOGE("mkfifo %s fail! (%s)\n", fileName.c_str(), strerror(errno));
        return;
    }

    mFifoFd = open(fileName.c_str(), O_RDWR | O_NONBLOCK);
    if (mFifoFd == -1) {
        SPR_LOGE("open %s fail! (%s)\n", fileName.c_str(), strerror(errno));
        return;
    }

    mEvtFd = mFifoFd;
}

PPipe::~PPipe()
{
    Close();
    mFifoFd = -1;
}

void* PPipe::EpollEvent(int fd, EpollType eType, void* arg)
{
    if (fd != mEvtFd) {
        SPR_LOGE("Invalid fd (%d)!\n", fd);
    }

    std::string buf;
    if (Read(fd, buf) < 0) {
        SPR_LOGE("Read error!\n");
    }

    if (mCb) {
        arg = arg ? arg : this;
        mCb(fd, buf, arg);
    }

    return nullptr;
}
