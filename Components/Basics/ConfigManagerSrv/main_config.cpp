/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_config.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/06/20
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "SprLog.h"
#include "CommonMacros.h"
#include "ConfigManager.h"
#include "ConfigManagerHub.h"
#include "SprMainInterface.h"

#define LOG_TAG "MainConfig"

SPR_ENTRY(
    SPR_LOGI("Main start!\n");
    ConfigManager theConfigManager;
    theConfigManager.Init();

    ConfigManagerHub theConfigManagerHub(SRV_NAME_CONFIG_MANAGER, &theConfigManager);
    theConfigManagerHub.InitializeHub();
);
