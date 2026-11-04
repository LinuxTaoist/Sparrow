/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_sparrow.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/11/25
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "SprLog.h"
#include "SprSystem.h"
#include "SprMainInterface.h"

#define LOG_TAG "MainSparrow"

SPR_ENTRY(
    SPR_LOGI("Main start!\n");
    SprSystem::GetInstance()->Init();
);
