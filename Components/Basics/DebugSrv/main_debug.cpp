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
#include <signal.h>
#include "SprLog.h"
#include "GeneralUtils.h"
#include "CommonMacros.h"
#include "DebugModule.h"
#include "SprProcPrepare.h"
#include "DebugModuleHub.h"
#include "BacktraceMacros.h"
#include "SprEpollSchedule.h"

using namespace InternalDefs;

#define LOG_TAG "MainDebug"

int main(int argc, const char *argv[])
{
    GeneralUtils::InitSignalHandler([](int signum) {
        SPR_LOGI("Receive signal: %d!\n", signum);

        switch (signum) {
            case MAIN_EXIT_SIGNUM:   // 用户自定义信号1
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

    DebugModule theDebugModule(MODULE_DEBUG, "DebugM");
    DebugModuleHub theDebugModuleHub(SRV_NAME_DEBUG_MODULE, &theDebugModule);

    SprProcPrepare::GetInstance()->Init(SRV_NAME_DEBUG_MODULE);
    theDebugModule.Initialize();
    theDebugModuleHub.InitializeHub();
    SprEpollSchedule::GetInstance()->EpollLoop();
    SPR_LOGI("Main exit!\n");
    return 0;
}