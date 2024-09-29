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
#include "SprMediator.h"
#include "SprMediatorHub.h"

#define SPR_LOGD(fmt, args...) LOGD("SprMediator", fmt, ##args)
#define SPR_LOGI(fmt, args...) LOGI("SprMediator", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("SprMediator", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("SprMediator", fmt, ##args)

int main(int argc, const char *argv[])
{
    GeneralUtils::InitSignalHandler([](int signum) {
	    SPR_LOGI("Receive signal: %d!\n", signum);
        switch (signum) {
            case MAIN_EXIT_SIGNUM:
                SprMediator::StopWork();
                break;
            default:
                break;
        }
    });

    SprMediator *pObj = SprMediator::GetInstance();
    SprMediatorHub theMediatorHub(SRV_NAME_MEDIATOR, pObj);
    theMediatorHub.InitializeHub();
    pObj->Init();
    pObj->EpollLoop();

    SPR_LOGI("Exit main!\n");
    return 0;
}