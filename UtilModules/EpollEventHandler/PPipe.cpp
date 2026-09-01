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
#include "PLog.h"
#include "PPipe.h"
#include "EpollEventHandler.h"

#define PLOG_TAG "PPipe"

PPipe::PPipe(int32_t fd, const std::function<void(ssize_t, std::string, void*)>& cb, void* arg)
    : IEpollEvent(fd, EPOLL_TYPE_PIPE, arg), mCb(cb)
{
    int32_t flags = fcntl(mEvtFd, F_GETFL, 0);
    fcntl(mEvtFd, F_SETFL, flags | O_NONBLOCK);
}

PPipe::PPipe(const std::string& fileName, const std::function<void(ssize_t, std::string, void*)>& cb, void* arg)
    : IEpollEvent(-1, EPOLL_TYPE_PIPE, arg), mFifoName(fileName), mCb(cb)
{
    bool isExist = IsExistFifo(fileName);
    if (!isExist && mkfifo(fileName.c_str(), 0666) == -1) {
        PLOGE("mkfifo %s fail! (%s)\n", fileName.c_str(), strerror(errno));
        SetReady(false);
    }

    mEvtFd = open(fileName.c_str(), O_RDWR | O_NONBLOCK);
    if (mEvtFd == -1) {
        PLOGE("open %s fail! (%s)\n", fileName.c_str(), strerror(errno));
        SetReady(false);
    }

    if (isExist) {
        std::string bytes;
        while (Read(bytes) > 0) {
            PLOGD("Clear fifo %s\n", fileName.c_str());
        }
    }
}

PPipe::~PPipe()
{
    Close();
    if (!mFifoName.empty()) {
        unlink(mFifoName.c_str());
    }
}

bool PPipe::IsExistFifo(const std::string& path) {
    if (path.empty()) {
        return false;
    }

    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        return false;
    }

    return S_ISFIFO(buffer.st_mode);
}

void* PPipe::EpollEvent(int32_t fd, EpollType eType, void* arg)
{
    if (fd != mEvtFd) {
        PLOGE("Invalid fd (%d)!\n", fd);
    }

    std::string buf;
    int32_t ret = Read(fd, buf);
    if (ret < 0) {
        PLOGE("Read error!\n");
    }

    if (mCb) {
        arg = arg ? arg : this;
        mCb(ret, buf, arg);
    }

    return nullptr;
}
