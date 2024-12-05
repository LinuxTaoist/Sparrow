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
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "PFile.h"
#include "EpollEventHandler.h"

#define SPR_LOGD(fmt, args...) printf("%4d PFile D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d PFile E: " fmt, __LINE__, ##args)

PFile::PFile(int fd, std::function<void(int, void*)> cb, void* arg)
    : IEpollEvent(fd, EPOLL_TYPE_FILE, arg), mFd(-1), mCb1(cb), mCb2(nullptr)
{
    int flags = fcntl(mEvtFd, F_GETFL, 0);
    fcntl(mEvtFd, F_SETFL, flags | O_NONBLOCK);
}

// Note: See epoll_ctl(2) for further details.
// EPERM  The target file fd does not support epoll.
// This error can occur if fd refers to, for example, a regular file or a directory.
PFile::PFile(const std::string fileName, std::function<void(int, ssize_t, std::string, void*)> cb,
    void* arg, int flags, mode_t mode)
    : IEpollEvent(-1, EPOLL_TYPE_FILE, arg), mCb1(nullptr), mCb2(cb)
{
    mFd = open(fileName.c_str(), flags | O_NONBLOCK, mode);
    if (mFd < 0) {
        SPR_LOGE("open %s failed! (%s)\n", fileName.c_str(), strerror(errno));
    }
    mEvtFd = mFd;
}

PFile::~PFile()
{
    if (mFd >= 0) {
        close(mFd);
        mFd = -1;
        mEvtFd = -1;
    }
}

void* PFile::EpollEvent(int fd, EpollType eType, void* arg)
{
    if (fd != mEvtFd) {
        SPR_LOGE("Invalid fd (%d)!\n", fd);
    }

    arg = arg ? arg : this;
    if (mCb1) {
        mCb1(fd, arg);
    } else if (mCb2) {
        std::string bytes;
        ssize_t size = Read(fd, bytes);
        mCb2(fd, size, bytes, arg);
    }

    return nullptr;
}
