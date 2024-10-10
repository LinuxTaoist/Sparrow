/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PPipe.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/14
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/10/10 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "PPipe.h"

#define SPR_LOGD(fmt, args...) printf("%4d PPipe D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d PPipe E: " fmt, __LINE__, ##args)

PPipe::PPipe(int fd, std::function<void(int, std::string, void*)> cb, void *arg)
    : IEpollEvent(fd, EPOLL_TYPE_PIPE, arg), mCb(cb)
{
}

PPipe::~PPipe()
{
}

void* PPipe::EpollEvent(int fd, EpollType eType, void* arg)
{
    if (fd != mEpollFd) {
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
