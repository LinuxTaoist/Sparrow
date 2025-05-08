/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_power.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/12/21
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/12/21 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <iostream>
#include <string>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "SprLog.h"
#include "Backtrace.h"
#include "GeneralUtils.h"
#include "CommonMacros.h"
#include "PowerManager.h"
#include "SprDebugNode.h"
#include "SprProcPrepare.h"
#include "PowerManagerHub.h"
#include "SprEpollSchedule.h"

using namespace std;
using namespace InternalDefs;

#define LOG_TAG "MainPower"

int main(int argc, const char *argv[])
{
    GeneralUtils::InitSignalHandler([](int signum) {
        SPR_LOGI("Receive signal: %d!\n", signum);

        switch (signum) {
            case MAIN_EXIT_SIGNUM:
                SprEpollSchedule::GetInstance()->ExitLoop();
                break;
            case SIGSEGV:
            case SIGBUS:
            case SIGILL:
            case SIGFPE:
            case SIGQUIT:
                PRINT_BACKTRACE(signum, 20);
                SprEpollSchedule::GetInstance()->ExitLoop();
                exit(EXIT_FAILURE);
                break;
            default:
                break;
        }
    });

    PowerManager thePowerManager(MODULE_POWERM, "PowerM");
    PowerManagerHub thePowerManagerHub(SRV_NAME_POWER_MANAGER, &thePowerManager);

    SprProcPrepare::GetInstance()->Init(SRV_NAME_POWER_MANAGER);
    thePowerManager.Initialize();
    thePowerManagerHub.InitializeHub();
    SprEpollSchedule::GetInstance()->EpollLoop();
    SPR_LOGI("Exit main!\n");
    return 0;
}
