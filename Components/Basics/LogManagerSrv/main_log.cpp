/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_log.cpp
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
#include <stdio.h>
#include <signal.h>
#include "GeneralUtils.h"
#include "CoreTypeDefs.h"
#include "LogManager.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)
#define SPR_LOGI(fmt, args...) printf("%s %4d MainLog I: " fmt, GeneralUtils::GetCurTimeStr().c_str(), __LINE__, ##args)
#define SPR_LOGD(fmt, args...) printf("%s %4d MainLog D: " fmt, GeneralUtils::GetCurTimeStr().c_str(), __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%s %4d MainLog W: " fmt, GeneralUtils::GetCurTimeStr().c_str(), __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%s %4d MainLog E: " fmt, GeneralUtils::GetCurTimeStr().c_str(), __LINE__, ##args)

int main(int argc, const char *argv[])
{
    LogManager theLogManager;

    GeneralUtils::InitSignalHandler([](int signum) {
	    SPR_LOGI("Receive signal: %d!\n", signum);
        switch (signum) {
            case MAIN_EXIT_SIGNUM:
                LogManager::StopWork();
                break;

            case SIGUSR2:   // 用户自定义信号2
            default:
                break;
        }
    });

    theLogManager.MainLoop();
    SPR_LOGI("Main exit!\n");
    return 0;
}
