/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_power.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/12/21
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "SprLog.h"
#include "CommonMacros.h"
#include "PowerManager.h"
#include "PowerManagerHub.h"
#include "SprMainInterface.h"

#define LOG_TAG "MainPower"

SPR_ENTRY(
    SPR_LOGI("Main start!\n");
    PowerManager thePowerManager;
    thePowerManager.Initialize();

    PowerManagerHub thePowerManagerHub(SRV_NAME_POWER_MANAGER, &thePowerManager);
    thePowerManagerHub.InitializeHub();
);
