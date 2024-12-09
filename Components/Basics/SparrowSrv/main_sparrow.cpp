/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_sparrow.cc
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
#include "SprSystem.h"
#include "GeneralUtils.h"
#include "CoreTypeDefs.h"
#include "SprEpollSchedule.h"

#define SPR_LOGI(fmt, args...) LOGI("MainSparrow", fmt, ##args)

int main(int argc, const char *argv[])
{
    GeneralUtils::InitSignalHandler([](int signum) {
	    SPR_LOGI("Receive signal: %d!\n", signum);

        switch (signum) {
            case MAIN_EXIT_SIGNUM:
                SprEpollSchedule::GetInstance()->ExitLoop();
                break;
            default:
                break;
        }
    });

    SprSystem::GetInstance()->Init();
    SprEpollSchedule::GetInstance()->EpollLoop();
    SPR_LOGI("Main exit!\n");
    return 0;
}
