/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_service.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/03/26
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/26 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <unistd.h>
#include <signal.h>
#include "GeneralUtils.h"
#include "CoreTypeDefs.h"
#include "ServiceManager.h"

using namespace GeneralUtils;

#define SPR_LOGI(fmt, args...) printf("%s %6d %12s I: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "MainSrv", __LINE__, ##args)
#define SPR_LOGD(fmt, args...) printf("%s %6d %12s D: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "MainSrv", __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%s %6d %12s W: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "MainSrv", __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%s %6d %12s E: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "MainSrv", __LINE__, ##args)

int main(int argc, char* argv[])
{
    InitSignalHandler([](int signum) {
	    SPR_LOGI("Receive signal: %d!\n", signum);
        switch (signum) {
            case MAIN_EXIT_SIGNUM:   // 用户自定义信号1
                ServiceManager::StopWork();
                break;
            default:
                break;
        }
    });

    ServiceManager theServiceManager;
    theServiceManager.StartWork();

    SPR_LOGI("Main exit!\n");
    return 0;
}

