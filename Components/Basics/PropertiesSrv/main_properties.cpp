/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_properties.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/03/13
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "SprLog.h"
#include "CommonMacros.h"
#include "PropertyManager.h"
#include "PropertyManagerHub.h"
#include "SprMainInterface.h"

#define LOG_TAG "MainProper"

SPR_ENTRY(
    SPR_LOGI("Main start!\n");
    PropertyManager* pProperM = PropertyManager::GetInstance();
    pProperM->Initialize();

    PropertyManagerHub thePropertyManagerHub(SRV_NAME_PROPERTY, pProperM);
    thePropertyManagerHub.InitializeHub();
);
