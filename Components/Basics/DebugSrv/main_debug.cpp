/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_debug.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/28
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "SprLog.h"
#include "CommonMacros.h"
#include "DebugModule.h"
#include "DebugModuleHub.h"
#include "SprMainInterface.h"

#define LOG_TAG "MainDebug"

SPR_ENTRY(
    SPR_LOGI("Main start!\n");
    DebugModule theDebugModule;
    theDebugModule.Initialize();

    DebugModuleHub theDebugModuleHub(SRV_NAME_DEBUG_MODULE, &theDebugModule);
    theDebugModuleHub.InitializeHub();
);