/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_monitor.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/02/20
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <signal.h>
#include "SprLog.h"
#include "GeneralUtils.h"
#include "CoreTypeDefs.h"
#include "CommonMacros.h"
#include "SprProcPrepare.h"
#include "BacktraceMacros.h"
#include "StatusMonitorManager.h"
#include "SprEpollSchedule.h"

using namespace InternalDefs;

#define LOG_TAG "MainMonitor"

int main(int argc, const char* argv[])
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

    SPR_LOGI("Main start!\n");
    StatusMonitorManager monitorMgr(MODULE_STATUS_MONITOR, "StatusM");
    SprProcPrepare::GetInstance()->Init(SRV_NAME_STATUS_MONITOR);
    monitorMgr.Initialize();
    SprEpollSchedule::GetInstance()->EpollLoop();
    SPR_LOGI("Main exit!\n");
    return 0;
}
