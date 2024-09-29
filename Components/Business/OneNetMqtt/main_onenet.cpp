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
#include "SprEpollSchedule.h"

using namespace InternalDefs;

#define SPR_LOGI(fmt, args...) LOGI("EntryOneNet", fmt, ##args)
#define SPR_LOGD(fmt, args...) LOGD("EntryOneNet", fmt, ##args)

// The entry of OneNet business plugin
extern "C" void PluginEntry(std::map<int, SprObserver*>& observers, SprContext& ctx)
{
    auto pOneDrv = OneNetDriver::GetInstance(MODULE_ONENET_DRIVER, "OneDrv");
    auto pOneMgr = OneNetManager::GetInstance(MODULE_ONENET_MANAGER, "OneMgr");

    observers[MODULE_ONENET_DRIVER] = pOneDrv;
    observers[MODULE_ONENET_MANAGER] = pOneMgr;
    SPR_LOGD("Load plug-in OneNet modules\n");
}