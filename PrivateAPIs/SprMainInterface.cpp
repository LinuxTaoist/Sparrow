/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMainEntry.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/10/12
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/10/12 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <signal.h>
#include "SprLog.h"
#include "GeneralUtils.h"
#include "CoreTypeDefs.h"
#include "CommonMacros.h"
#include "SprProcInfo.h"
#include "SprProcPrepare.h"
#include "BacktraceMacros.h"
#include "SprEpollSchedule.h"
#include "SprMainInterface.h"

#define LOG_TAG "SprMainEntry"

void (*pSprUserEntry)() = nullptr;

void SprRunEventLoop()
{
    SprEpollSchedule::GetInstance()->EpollLoop();
}

static void SPR_ENTRY_IMPLEMENT_CHECK() {
    NOT_IMPLEMENT_SPR_ENTRY();
}

int main(int argc, const char* argv[])
{
    SPR_ENTRY_IMPLEMENT_CHECK();

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

    std::string proc = SprProcInfo::GetInstance()->GetProcName();
    SprProcPrepare::GetInstance()->Init(proc);
    pSprUserEntry();
    SPR_LOGI("Main exit!\n");
    return 0;
}
