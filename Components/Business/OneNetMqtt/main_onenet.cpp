/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_onenet.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/12/21
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/12/21 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <signal.h>
#include "SprLog.h"
#include "CommonMacros.h"
#include "GeneralUtils.h"
#include "OneNetManager.h"
#include "OneNetDriver.h"
#include "SprEpollSchedule.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGI(fmt, args...) LOGI("MainOneNet", fmt, ##args)
#define SPR_LOGD(fmt, args...) LOGD("MainOneNet", fmt, ##args)

int main(int argc, const char *argv[])
{
    GeneralUtils::InitSignalHandler([](int signum) {
	    SPR_LOGI("Receive signal: %d!\n", signum);
        switch (signum) {
            case MAIN_EXIT_SIGNUM:   // 用户自定义信号1
                SprEpollSchedule::GetInstance()->ExitLoop();
                break;

            case SIGUSR2:   // 用户自定义信号2
            default:
                break;
        }
    });

    OneNetDriver::GetInstance(MODULE_ONENET_DRIVER, "OneDrv")->Initialize();
    OneNetManager::GetInstance(MODULE_ONENET_MANAGER, "OneMgr")->Initialize();
    SprEpollSchedule::GetInstance()->EpollLoop(true);
    SPR_LOGI("Main exit!\n");
    return 0;
}