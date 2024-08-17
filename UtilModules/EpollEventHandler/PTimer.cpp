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
    mTimerRun = false;
    InitTimer();
}

PTimer::~PTimer()
{
    DestoryTimer();
}

int32_t PTimer::InitTimer()
{
    mEpollFd = timerfd_create(CLOCK_REALTIME, 0);
    if (mEpollFd == -1) {
        SPR_LOGE("timerfd_create failed! (%s)\n", strerror(errno));
        return -1;
    }

    return mEpollFd;
}
int32_t PTimer::StartTimer(uint32_t delayInMSec, uint32_t intervalInMSec)
{
    if (mTimerRun) {
        SPR_LOGW("System timer is running!\n");
        return 0;
    }

    struct itimerspec its;
    its.it_value.tv_sec = delayInMSec / 1000;
    its.it_value.tv_nsec = (delayInMSec % 1000) * 1000000;
    its.it_interval.tv_sec = intervalInMSec / 1000;
    its.it_interval.tv_nsec = (intervalInMSec % 1000) * 1000000;
    if (timerfd_settime(mEpollFd, 0, &its, nullptr) == -1) {
        SPR_LOGE("timerfd_settime fail! (%s)\n", strerror(errno));
        return -1;
    }

    mTimerRun = true;
    SPR_LOGD("Start system timer (%d)!\n", intervalInMSec);
    return 0;
}

int32_t PTimer::StopTimer()
{
    struct itimerspec its;
    its.it_value.tv_sec     = 0;
    its.it_value.tv_nsec    = 0;
    its.it_interval.tv_sec  = 0;
    its.it_interval.tv_nsec = 0;

    if (timerfd_settime(mEpollFd, 0, &its, nullptr) == -1) {
        SPR_LOGE("timerfd_settime fail! fd = %d (%s)\n", mEpollFd, strerror(errno));
        return -1;
    }

    mTimerRun = false;
    SPR_LOGD("Stop system timer!\n");
    return 0;
}

int32_t PTimer::DestoryTimer()
{
    if (mEpollFd != -1) {
        close(mEpollFd);
        mEpollFd = -1;
    }

    SPR_LOGD("Destory system timer!");
    return 0;
}

void* PTimer::EpollEvent(int fd, EpollType eType, void* arg)
{
    if (fd != mEpollFd) {
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