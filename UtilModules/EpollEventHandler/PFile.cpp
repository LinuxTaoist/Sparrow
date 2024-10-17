/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PFile.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/10/17
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/10/17 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "PFile.h"

#define SPR_LOGD(fmt, args...) printf("%4d PFile D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d PFile E: " fmt, __LINE__, ##args)

PFile::PFile(int fd, std::function<void(int, void*)> cb, void *arg)
    : IEpollEvent(fd, EPOLL_TYPE_FILE, arg), mCb(cb)
{
}

PFile::~PFile()
{
}

void* PFile::EpollEvent(int fd, EpollType eType, void* arg)
{
    if (fd != mEpollFd) {
        SPR_LOGE("Invalid fd (%d)!\n", fd);
    }

    if (mCb) {
        arg = arg ? arg : this;
        mCb(fd, arg);
    }

    return nullptr;
}
