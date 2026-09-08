/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CustomDebugWatch.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/04/23
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/04/23 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __CUSTOM_DEBUG_WATCH_H__
#define __CUSTOM_DEBUG_WATCH_H__

#include <thread>
#include "BaseWatch.h"

class CustomDebugWatch : public BaseWatch {
public:
    static CustomDebugWatch& GetInstance();

private:
    CustomDebugWatch() = default;
    ~CustomDebugWatch() override = default;
    void Usage() override;
    void Menu(char input) override;

    void HandleAddTimerInOneSec();
    void HandleDelTimerInOneSec();
    void HandleAddCustomTimer();
    void HandleDelCustomTimer();
    void HandleEnableRemoteShell();
    void HandleDisableRemoteShell();
};

#endif // __CUSTOM_DEBUG_WATCH_H__
