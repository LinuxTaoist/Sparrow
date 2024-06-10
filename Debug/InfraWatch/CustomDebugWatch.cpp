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

CustomDebugWatch theCustomDebugWatch;

char CustomDebugWatch::MenuEntry()
{
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

    char input = InfraWatch::WaitUserInputWithoutEnter();
    HandleInputInMenu(input);
    return input;
}

char CustomDebugWatch::HandleInputInMenu(char input)
{
    switch(input)
    {
        case '1':
        {
            DebugInterface::GetInstance()->AddTimerInOneSec();
            break;
        }

        case '2':
        {
            DebugInterface::GetInstance()->DelTimerInOneSec();
            break;
        }

        case '3':
        {
            uint32_t repeatTimes;
            int32_t delayInMilliSec;
            int32_t intervalInMilliSec;

            SPR_LOG("Input timer settings: <repetitions> <delay ms> <interval ms> \n");
            int ret = scanf("%d %d %d", &repeatTimes, &delayInMilliSec, &intervalInMilliSec);
            if (ret != 3)
            {
                SPR_LOG("Invalid input! (%s)\n", strerror(errno));
                break;
            }

            DebugInterface::GetInstance()->AddCustomTimer(repeatTimes, delayInMilliSec, intervalInMilliSec);
            break;
        }

        case '4':
        {
            DebugInterface::GetInstance()->DelCustomTimer();
            break;
        }

        case '5':
        {
            DebugInterface::GetInstance()->EnableRemoteShell();
            break;
        }

        case '6':
        {
            DebugInterface::GetInstance()->DisableRemoteShell();
            break;
        }

        case 'q':
        {
            break;
        }

        default:
            break;
    }

    return 0;
}
