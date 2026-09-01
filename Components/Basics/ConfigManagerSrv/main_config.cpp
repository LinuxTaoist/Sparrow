/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_config.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/06/20
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/06/20 | 1.0.0.1   | Xiang.D        | Create file
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
#include "ConfigManager.h"
#include "ConfigManagerHub.h"

using namespace InternalDefs;

#define LOG_TAG "MainConfig"

int main(int argc, char*  argv[])
{
    GeneralUtils::InitSignalHandler([](int signum) {
        SPR_LOGI("Receive signal: %d!\n", signum);
        switch (signum) {
            case MAIN_EXIT_SIGNUM:
                ConfigManager::StopWork();
                break;
            case SIGSEGV:
            case SIGBUS:
            case SIGILL:
            case SIGFPE:
            case SIGQUIT:
                PRINT_BACKTRACE(signum, 20);
                ConfigManager::StopWork();
                exit(EXIT_FAILURE);
                break;
            default:
                break;
        }
    });

    SPR_LOGI("Main start!\n");
    ConfigManager theConfigManager;
    ConfigManagerHub theConfigManagerHub(SRV_NAME_CONFIG_MANAGER, &theConfigManager);

    SprProcPrepare::GetInstance()->Init(SRV_NAME_CONFIG_MANAGER);
    if (theConfigManager.Initialize() != 0) {
        SPR_LOGE("ConfigManager initialize failed!\n");
        return -1;
    }

    theConfigManagerHub.InitializeHub();
    theConfigManager.MainLoop();
    SPR_LOGI("Exit main!\n");
    return 0;
}