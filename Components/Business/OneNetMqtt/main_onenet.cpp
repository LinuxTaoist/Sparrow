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

using namespace InternalDefs;

#define SPR_LOGI(fmt, args...) LOGI("EntryOneNet", fmt, ##args)
#define SPR_LOGD(fmt, args...) LOGD("EntryOneNet", fmt, ##args)

// The entry of OneNet business plugin
extern "C" void PluginEntry(int32_t& index, std::string& desc)
{
    index += 1;
    desc = "OneNet Business Plugin";
    OneNetDriver::GetInstance(MODULE_ONENET_DRIVER, "OneDrv")->Initialize();
    OneNetManager::GetInstance(MODULE_ONENET_MANAGER, "OneMgr")->Initialize();
    SPR_LOGD("PluginEntry: index = %d, desc = %s\n", index, desc.c_str());
}