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
#include <stdio.h>
#include <signal.h>
#include "SprLog.h"
#include "GeneralUtils.h"
#include "CommonMacros.h"
#include "PowerManager.h"
#include "SprDebugNode.h"
#include "PowerManagerHub.h"
#include "SprEpollSchedule.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGI(fmt, args...) LOGI("MainPower", fmt, ##args)

int main(int argc, const char *argv[])
{
    PowerManager thePowerManager(MODULE_POWERM, "PowerM");
    PowerManagerHub thePowerManagerHub(SRV_NAME_POWER_MANAGER, &thePowerManager);
    thePowerManager.Initialize();
    thePowerManagerHub.InitializeHub();

    GeneralUtils::InitSignalHandler([](int signum) {
	    SPR_LOGI("Receive signal: %d!\n", signum);

        switch (signum) {
            case MAIN_EXIT_SIGNUM:
                SprEpollSchedule::GetInstance()->ExitLoop();
                break;
            default:
                break;
        }
    });

    SprDebugNode::GetInstance()->InitPipeDebugNode(string("/tmp/") + SRV_NAME_POWER_MANAGER);
    SprEpollSchedule::GetInstance()->EpollLoop();
    SPR_LOGI("Exit main!\n");
    return 0;
}
