/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_monitor.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/02/20
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "SprLog.h"
#include "CommonMacros.h"
#include "StatusMonitorManager.h"
#include "StatusMonitorHub.h"
#include "SprMainInterface.h"

#define LOG_TAG "MainMonitor"

SPR_ENTRY(
    SPR_LOGI("Main start!\n");
    StatusMonitorManager monitorMgr;
    monitorMgr.Initialize();

    StatusMonitorHub theStatusMonitorHub(SRV_NAME_STATUS_MONITOR, &monitorMgr);
    theStatusMonitorHub.InitializeHub();
);
