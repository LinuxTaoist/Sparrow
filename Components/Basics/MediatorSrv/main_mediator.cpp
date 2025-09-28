/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_proxy.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/11/25
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
#include "CommonMacros.h"
#include "GeneralUtils.h"
#include "SprProcPrepare.h"
#include "SprMediator.h"
#include "SprMediatorHub.h"
#include "BacktraceMacros.h"
#include "EpollEventHandler.h"

#define LOG_TAG "MainMediator"

int main(int argc, const char* argv[])
{
    GeneralUtils::InitSignalHandler([](int signum) {
        SPR_LOGI("Receive signal: %d!\n", signum);
        switch (signum) {
            case MAIN_EXIT_SIGNUM:
                EpollEventHandler::GetInstance()->ExitLoop();
                break;
            case SIGSEGV:
            case SIGBUS:
            case SIGILL:
            case SIGFPE:
            case SIGQUIT:
                PRINT_BACKTRACE(signum, 20);
                EpollEventHandler::GetInstance()->ExitLoop();
                exit(EXIT_FAILURE);
                break;
            default:
                break;
        }
    });

    SprMediator *pMedObj = SprMediator::GetInstance();
    SprMediatorHub theMediatorHub(SRV_NAME_MEDIATOR, pMedObj);

    SprProcPrepare::GetInstance()->Init(SRV_NAME_MEDIATOR);
    theMediatorHub.InitializeHub();
    pMedObj->Init();
    EpollEventHandler::GetInstance()->EpollLoop();
    SPR_LOGI("Exit main!\n");
    return 0;
}