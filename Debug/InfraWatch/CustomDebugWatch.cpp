/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CustomDebugWatch.cpp
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
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "InfraCommon.h"
#include "DebugInterface.h"
#include "CustomDebugWatch.h"

using namespace std;

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

CustomDebugWatch& CustomDebugWatch::GetInstance() {
    static CustomDebugWatch instance;
    return instance;
}

void CustomDebugWatch::Usage() {
    InfraWatch::ClearScreen();
    SPR_LOG("================================  Custom  Debug  ================================\n"
            "\n"
            "    1. AddTimerInOneSec    \n"
            "    2. DelTimerInOneSec    \n"
            "    3. AddCustomTimer      \n"
            "    4. DelCustomTimer      \n"
            "    5. EnableRemoteShell   \n"
            "    6. DisableRemoteShell  \n"
            "\n"
            "    [Q] Quit\n"
            "\n"
            "=================================================================================\n");
}

void CustomDebugWatch::Menu(char input) {
    switch(input) {
        case '1': {
            HandleAddTimerInOneSec();
            break;
        }
        case '2': {
            HandleDelTimerInOneSec();
            break;
        }
        case '3': {
            HandleAddCustomTimer();
            break;
        }
        case '4': {
            HandleDelCustomTimer();
            break;
        }
        case '5': {
            HandleEnableRemoteShell();
            break;
        }
        case '6': {
            HandleDisableRemoteShell();
            break;
        }
        case 'q': {
            break;
        }
        default:
            break;
    }
}

void CustomDebugWatch::HandleAddTimerInOneSec() {
    DebugInterface::GetInstance()->AddTimerInOneSec();
}

void CustomDebugWatch::HandleDelTimerInOneSec() {
    DebugInterface::GetInstance()->DelTimerInOneSec();
}

void CustomDebugWatch::HandleAddCustomTimer() {
    uint32_t repeatTimes;
    int32_t delayInMilliSec;
    int32_t intervalInMilliSec;

    SPR_LOG("Input timer settings: <repetitions> <delay ms> <interval ms> \n");
    int ret = scanf("%u %d %d", &repeatTimes, &delayInMilliSec, &intervalInMilliSec);
    if (ret != 3) {
        SPR_LOG("Invalid input! (%s)\n", strerror(errno));
        return;
    }

    DebugInterface::GetInstance()->AddCustomTimer(repeatTimes, delayInMilliSec, intervalInMilliSec);
}

void CustomDebugWatch::HandleDelCustomTimer() {
    DebugInterface::GetInstance()->DelCustomTimer();
}

void CustomDebugWatch::HandleEnableRemoteShell() {
    DebugInterface::GetInstance()->EnableRemoteShell();
}

void CustomDebugWatch::HandleDisableRemoteShell() {
    DebugInterface::GetInstance()->DisableRemoteShell();
}
