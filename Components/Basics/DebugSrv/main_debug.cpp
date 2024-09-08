/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_debug.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/28
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/28 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <memory>
#include <stdio.h>
#include <signal.h>
#include "SprLog.h"
#include "GeneralUtils.h"
#include "CommonMacros.h"
#include "DebugModule.h"
#include "DebugModuleHub.h"
#include "SprEpollSchedule.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGI(fmt, args...) LOGI("MainDebug", fmt, ##args)

int main(int argc, const char *argv[])
{
    DebugModule theDebugModule(MODULE_DEBUG, "DebugM");
    DebugModuleHub theDebugModuleHub(SRV_NAME_DEBUG_MODULE, &theDebugModule);

    GeneralUtils::InitSignalHandler([](int signum) {
	    SPR_LOGI("Receive signal: %d!\n", signum);

        switch (signum) {
            case MAIN_EXIT_SIGNUM:   // 用户自定义信号1
                SprEpollSchedule::GetInstance()->ExitLoop();
                break;
            default:
                break;
        }
    });

    theDebugModuleHub.InitializeHub();
    SprEpollSchedule::GetInstance()->EpollLoop(true);

    SPR_LOGI("Main exit!\n");
    return 0;
}