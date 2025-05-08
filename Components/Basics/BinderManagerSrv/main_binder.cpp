/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_binder.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/03/16
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/16 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <iostream>
#include <string>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include "SprLog.h"
#include "Backtrace.h"
#include "CommonMacros.h"
#include "GeneralUtils.h"
#include "CoreTypeDefs.h"
#include "BinderManager.h"

using namespace GeneralUtils;

#define LOG_TAG "MainBinder"

int main(int argc, const char *argv[])
{
    InitSignalHandler([](int signum) {
        SPR_LOGI("Receive signal: %d!\n", signum);
        switch (signum) {
            case MAIN_EXIT_SIGNUM:
                BinderManager::StopWork();
                break;
            case SIGSEGV:
            case SIGBUS:
            case SIGILL:
            case SIGFPE:
            case SIGQUIT:
                PRINT_BACKTRACE(signum, 20);
                BinderManager::StopWork();
                exit(EXIT_FAILURE);
                break;
            default:
                break;
        }
    });

    BinderManager::GetInstance()->StartWork();
    SPR_LOGI("Main Exit!\n");
    return 0;
}
