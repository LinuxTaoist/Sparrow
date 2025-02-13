/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PTimer.cpp
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
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include "PTimer.h"

#define SPR_LOGD(fmt, args...) printf("%4d PTimer D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%4d PTimer W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d PTimer E: " fmt, __LINE__, ##args)

PTimer::PTimer(std::function<void(int, uint64_t, void*)> cb, void *arg)
    : IEpollEvent(-1, EPOLL_TYPE_TIMERFD, arg), mCb(cb)
{
    InitTimer();
}

PTimer::~PTimer()
{
    DestoryTimer();
}

int32_t PTimer::InitTimer()
{
    mEvtFd = timerfd_create(CLOCK_REALTIME, 0);
    if (mEvtFd == -1) {
        SPR_LOGE("timerfd_create failed! (%s)\n", strerror(errno));
        return -1;
    }

    return mEvtFd;
}

int32_t PTimer::StartTimer(uint32_t delayInMSec, uint32_t intervalInMSec)
{
    struct itimerspec its;
    its.it_value.tv_sec = delayInMSec / 1000;
    its.it_value.tv_nsec = (delayInMSec % 1000) * 1000000;
    its.it_interval.tv_sec = intervalInMSec / 1000;
    its.it_interval.tv_nsec = (intervalInMSec % 1000) * 1000000;
    if (timerfd_settime(mEvtFd, 0, &its, nullptr) == -1) {
        SPR_LOGE("timerfd_settime fail! (%s)\n", strerror(errno));
        return -1;
    }

    // SPR_LOGD("Start system timer (%d %d)!\n", delayInMSec, intervalInMSec);
    return 0;
}

int32_t PTimer::StopTimer()
{
    struct itimerspec its;
    its.it_value.tv_sec     = 0;
    its.it_value.tv_nsec    = 0;
    its.it_interval.tv_sec  = 0;
    its.it_interval.tv_nsec = 0;

    if (timerfd_settime(mEvtFd, 0, &its, nullptr) == -1) {
        SPR_LOGE("timerfd_settime fail! fd = %d (%s)\n", mEvtFd, strerror(errno));
        return -1;
    }

    SPR_LOGD("Stop system timer!\n");
    return 0;
}

int32_t PTimer::DestoryTimer()
{
    if (mEvtFd != -1) {
        close(mEvtFd);
        mEvtFd = -1;
    }

    SPR_LOGD("Destory system timer!\n");
    return 0;
}

ssize_t PTimer::Read(int fd, std::string& bytes)
{
    uint64_t exp;
    ssize_t rc = read(fd, &exp, sizeof(exp));
    if (rc != sizeof(uint64_t)) {
        SPR_LOGE("read fail! (%s)\n", strerror(errno));
        return rc;
    }

    bytes.push_back(exp & 0xFF);
    bytes.push_back((exp >> 8 ) & 0xFF);
    bytes.push_back((exp >> 16) & 0xFF);
    bytes.push_back((exp >> 24) & 0xFF);
    bytes.push_back((exp >> 32) & 0xFF);
    bytes.push_back((exp >> 40) & 0xFF);
    bytes.push_back((exp >> 48) & 0xFF);
    bytes.push_back((exp >> 56) & 0xFF);
    return rc;
}

void* PTimer::EpollEvent(int fd, EpollType eType, void* arg)
{
    if (fd != mEvtFd) {
        SPR_LOGE("Invalid fd (%d)!\n", fd);
    }

    uint64_t exp;
    ssize_t rc = read(fd, &exp, sizeof(exp));
    if (rc != sizeof(uint64_t)) {
        SPR_LOGE("read fail! (%s)\n", strerror(errno));
        return nullptr;
    }

    if (mCb) {
        arg = arg ? arg : this;
        mCb(fd, exp, arg);
    }

    return nullptr;
}