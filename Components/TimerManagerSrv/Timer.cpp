/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Timer.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/12/15
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *------------------------------------------------------------------------------
 *  2023/12/15 | 1.0.0.1   | Xiang.D        | Create file
 *------------------------------------------------------------------------------
 *
 */
#include <iostream>
#include <cstring>
#include "Timer.h"

#define SPR_LOGD(fmt, ...) std::cout << __LINE__ << " Timer D: " << fmt << std::endl
#define SPR_LOGW(fmt, ...) std::cout << __LINE__ << " Timer W: " << fmt << std::endl
#define SPR_LOGE(fmt, ...) std::cout << __LINE__ << " Timer E: " << fmt << std::endl

SprTimer* curTimer = nullptr;

SprTimer::SprTimer(TimerCallback callback) :
    mRunning(false), mTimerID(0), mCallback(std::move(callback))
{
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &SprTimer::CallbackWrapper;

    if (sigaction(SIGALRM, &sa, nullptr) == -1) {
        SPR_LOGE("Failed to set sigaction for timer. (%s)\n", std::strerror(errno));
    }

    curTimer = this;
}

SprTimer::~SprTimer()
{
    if (mRunning) {
        Stop();
    }
}

int SprTimer::Start(const int& delayMs, const int& intervalMs)
{
    if (mRunning) {
        SPR_LOGE("Timer is already running\n");
        return -1;
    }

    struct sigevent sev;
    std::memset(&sev, 0, sizeof(struct sigevent));
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGALRM;
    sev.sigev_value.sival_ptr = this;

    if (timer_create(CLOCK_REALTIME, &sev, &mTimerID) == -1) {
        SPR_LOGE("Failed to create timer. (%s)\n", std::strerror(errno));
        return -1;
    }

    struct itimerspec its;
    // 设置第一次触发时间
    if (delayMs >= 0) {
        its.it_value.tv_sec = delayMs / 1000;
        its.it_value.tv_nsec = delayMs * 1000000;
    }

    // 设置周期时间
    if (intervalMs >= 0) {
        its.it_interval.tv_sec = intervalMs / 1000;
        its.it_interval.tv_nsec = intervalMs * 1000000;
    }

    if (timer_settime(mTimerID, 0, &its, NULL) == -1) {
        SPR_LOGE("Failed to start timer. (%s)\n", std::strerror(errno));
        return -1;
    }

    mRunning = true;
    return 0;
}

int SprTimer::Stop()
{
    if (!mRunning) {
        SPR_LOGW("Timer is not running\n");
        return -1;
    }

    struct itimerspec its;
    std::memset(&its, 0, sizeof(struct itimerspec));

    if (timer_settime(mTimerID, 0, &its, NULL) == -1) {
        SPR_LOGE("Failed to stop timer. (%s)\n", std::strerror(errno));
        return -1;
    }

    mRunning = false;
    return 0;
}

int SprTimer::Update(const int& intervalMs)
{
    if (!mRunning) {
        SPR_LOGE("Timer is not running\n");
        return -1;
    }

    struct itimerspec its;
    // 获取剩余的第一次触发时间
    struct itimerspec remaining;
    if (timer_gettime(mTimerID, &remaining) == -1) {
        SPR_LOGE("Failed to get remaining time for timer. (%s)\n", std::strerror(errno));
        return -1;
    }

    // 设置周期时间
    if (intervalMs > 0) {
        its.it_interval.tv_sec = intervalMs / 1000;
        its.it_interval.tv_nsec = (intervalMs % 1000) * 1000000;
    } else {
        its.it_interval.tv_sec = 0;
        its.it_interval.tv_nsec = 0;
    }

    // 恢复剩余的第一次触发时间
    its.it_value = remaining.it_value;
    if (timer_settime(mTimerID, 0, &its, NULL) == -1) {
        SPR_LOGE("Failed to update timer interval. (%s)\n", std::strerror(errno));
        return -1;
    }

    return 0;
}

void SprTimer::CallbackWrapper(int /* signum */, siginfo_t* si, void *)
{
    SprTimer* timer = static_cast<SprTimer*>(si->si_value.sival_ptr);
    timer->mCallback();
}
