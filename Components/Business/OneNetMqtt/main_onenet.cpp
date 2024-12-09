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
#include "SprContext.h"
#include "CommonMacros.h"
#include "GeneralUtils.h"
#include "OneNetManager.h"
#include "OneNetDriver.h"
#include "OneNetHub.h"
#include "SprEpollSchedule.h"

using namespace InternalDefs;

#define SPR_LOGI(fmt, args...) LOGI("EntryOneNet", fmt, ##args)
#define SPR_LOGD(fmt, args...) LOGD("EntryOneNet", fmt, ##args)

OneNetHub* gpOneNetHub = nullptr;

// The entry of OneNet business plugin
extern "C" void PluginEntry(std::map<int, SprObserver*>& observers, SprContext& ctx)
{
    if (observers.find(MODULE_ONENET_DRIVER) != observers.end() && observers[MODULE_ONENET_DRIVER]) {
        SPR_LOGD("OneNet driver module has been loaded!\n");
        return;
    }

    if (observers.find(MODULE_ONENET_MANAGER) != observers.end() && observers[MODULE_ONENET_MANAGER]) {
        SPR_LOGD("OneNet manager module has been loaded!\n");
        return;
    }

    auto pOneDrv = new (std::nothrow) OneNetDriver(MODULE_ONENET_DRIVER, "OneDrv");
    auto pOneMgr = new (std::nothrow) OneNetManager(MODULE_ONENET_MANAGER, "OneMgr");
    gpOneNetHub = new (std::nothrow) OneNetHub("OneNetMqtt", pOneMgr);

    pOneDrv->Initialize();
    pOneMgr->Initialize();
    gpOneNetHub->InitializeHub();
    observers[MODULE_ONENET_DRIVER] = pOneDrv;
    observers[MODULE_ONENET_MANAGER] = pOneMgr;
    SPR_LOGD("Load plug-in OneNet modules\n");
}

// The exit of OneNet business plugin
extern "C" void PluginExit(std::map<int, SprObserver*>& observers, SprContext& ctx)
{
    if (gpOneNetHub) {
        delete gpOneNetHub;
        gpOneNetHub = nullptr;
    }

    auto it = observers.find(MODULE_ONENET_DRIVER);
    if (it != observers.end() && it->second) {
        delete it->second;
        it->second = nullptr;
        observers.erase(it);
    }

    it = observers.find(MODULE_ONENET_MANAGER);
    if (it != observers.end() && it->second) {
        delete it->second;
        it->second = nullptr;
        observers.erase(it);
    }

    SPR_LOGD("Unload plug-in OneNet modules\n");
}