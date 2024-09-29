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
#include <unistd.h>
#include "SprLog.h"
#include "GeneralUtils.h"
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
#include "BindInterface.h"
#include "CoreTypeDefs.h"
#include "PropertyManager.h"
#include "PropertyManagerHub.h"

using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("MainProper", fmt, ##args)
#define SPR_LOGI(fmt, args...) LOGI("MainProper", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("MainProper", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("MainProper", fmt, ##args)

static bool wait = true;

int main(int argc, char * argv[])
{
    PropertyManager* pProperM = PropertyManager::GetInstance();
    PropertyManagerHub thePropertyManagerHub("property_service", pProperM);

    GeneralUtils::InitSignalHandler([](int signum) {
	    SPR_LOGI("Receive signal: %d!\n", signum);
        switch (signum) {
            case MAIN_EXIT_SIGNUM:
                wait = false;
                break;
            default:
                break;
        }
    });

    pProperM->Init();
    thePropertyManagerHub.InitializeHub();

    while (wait) {
        sleep(1);
    }

    SPR_LOGI("Exit main!\n");
    return 0;
}
