/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Timer.h
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
#ifndef __SPR_TIMER_H__
#define __SPR_TIMER_H__

#include <functional>
#include <cstring>
#include <ctime>
#include <csignal>

using TimerCallback = std::function<void()>;

class SprTimer
{
public:
    SprTimer(TimerCallback callback);
    ~SprTimer();

    /**
     * @brief       启动定时器
     *
     * @param[in]   delayMs    延时启动时间(毫秒)
     * @param[in]   intervalMs 定时器周期(毫秒) 0, 设置为一次性定时器; 其他值, 触发周期
     */
    int Start(const int& delayMs, const int& intervalMs);
    int Stop();

    /**
     * @brief       更新定时器周期
     *
     * @param[in]   intervalMs 定时器周期(毫秒) 0, 调整为一次性定时器; 其他值, 更新周期
     */
    int Update(const int& intervalMs);

private:
    bool mRunning;
    timer_t mTimerID;
    TimerCallback mCallback;

    static void CallbackWrapper(int /* signum */, siginfo_t* si, void *);
};

#endif  // __SPR_TIMER_H__