/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_properties.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/03/13
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/13 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <signal.h>
#include "SprLog.h"
#include "GeneralUtils.h"
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
#include "SprProcPrepare.h"
#include "BacktraceMacros.h"
#include "PropertyManager.h"
#include "PropertyManagerHub.h"
#include "EpollEventHandler.h"

using namespace InternalDefs;

#define LOG_TAG "MainProper"

int main(int argc, char*  argv[])
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

    SPR_LOGI("Main start!\n");
    PropertyManager* pProperM = PropertyManager::GetInstance(MODULE_PROPERTYM, "ProperM");
    PropertyManagerHub thePropertyManagerHub("property_service", pProperM);

    SprProcPrepare::GetInstance()->Init(SRV_NAME_PROPERTY);
    pProperM->Initialize();
    thePropertyManagerHub.InitializeHub();

    EpollEventHandler::GetInstance()->EpollLoop();
    SPR_LOGI("Exit main!\n");
    return 0;
}
