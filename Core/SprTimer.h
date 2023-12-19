/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprTimer.h
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

using TimerCallback = std::function<void()>;

class SprTimer
{
public:
    SprTimer(TimerCallback callback);
    ~SprTimer();

    int Start(const int& delay, const int& interval);
    int Stop();

    /**
     * @brief       更新定时器周期
     *
     * @param[in]   interval 定时器周期 其他值, 更新周期; 0, 调整为一次性定时器
     */

    int UpdateInterval(const int& interval);

private:
    bool mRunning;
    TimerCallback mCallback;

    static void CallbackWrapper(int signum);
};

#endif  // __SPR_TIMER_H__
