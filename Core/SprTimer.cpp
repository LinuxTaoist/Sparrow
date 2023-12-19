/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprTimer.cpp
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
#include <chrono>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include "SprTimer.h"

#define SPR_LOGD(fmt, ...) printf("%d Timer D: " fmt "\n", __LINE__, ##__VA_ARGS__)
#define SPR_LOGW(fmt, ...) printf("%d Timer W: " fmt "\n", __LINE__, ##__VA_ARGS__)
#define SPR_LOGE(fmt, ...) printf("%d Timer E: " fmt "\n", __LINE__, ##__VA_ARGS__)

SprTimer* curTimer = nullptr;

SprTimer::SprTimer(TimerCallback callback) :
    mRunning(false), mCallback(std::move(callback))
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = &SprTimer::CallbackWrapper;
    sigaction(SIGALRM, &sa, nullptr);
    curTimer = this;
}

SprTimer::~SprTimer()
{
    if (mRunning) {
        Stop();
    }
}

int SprTimer::Start(const int& delay, const int& interval)
{
    if (mRunning) {
        SPR_LOGE("Timer is already running\n");
        return -1;
    }

    struct itimerval timer;

    // 设置第一次触发时间
    if (delay > 0) {
        timer.it_value.tv_sec = delay;
        timer.it_value.tv_usec = 0;
    } else {
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 1;
    }

    // 设置周期时间
    timer.it_interval.tv_sec = interval;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timer, nullptr) == -1) {
        SPR_LOGE("Fail to start timer. (%s)\n", strerror(errno));
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

    struct itimerval timer;
    memset(&timer, 0, sizeof(struct itimerval));

    mRunning = false;
    if (setitimer(ITIMER_REAL, &timer, nullptr) == -1) {
        SPR_LOGE("Fail to setitimer. (%s)\n", strerror(errno));
        return -1;
    }

    return 0;
}

int SprTimer::UpdateInterval(const int& interval)
{
    int ret = 0;
    if (mRunning) {
        struct itimerval timer;

        // 获取剩余的第一次触发时间
        struct itimerval remaining;
        if (getitimer(ITIMER_REAL, &remaining) == -1) {
            SPR_LOGE("Fail to getitimer. (%s)\n", strerror(errno));
            return -1;
        }

        // 设置周期时间
        timer.it_interval.tv_sec = interval;
        timer.it_interval.tv_usec = 0;

        // 恢复剩余的第一次触发时间
        timer.it_value = remaining.it_value;
        if (setitimer(ITIMER_REAL, &timer, nullptr) == -1) {
            SPR_LOGE("Fail to setitimer. (%s)\n", strerror(errno));
            ret = -1;
        }
    } else {
        SPR_LOGE("Timer is not running\n");
        ret = -1;
    }

    return ret;
}

void SprTimer::CallbackWrapper(int /* signum */)
{
    curTimer->mCallback();
}

