/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_mediator.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/11/25
 *---------------------------------------------------------------------------------------------------------------------
 */

#include "SprLog.h"
#include "CommonMacros.h"
#include "SprMediator.h"
#include "SprMediatorHub.h"
#include "SprMainInterface.h"

#define LOG_TAG "MainMediator"

SPR_ENTRY(
    SPR_LOGI("Main start!\n");
    SprMediator* pMedObj = SprMediator::GetInstance();
    SprMediatorHub theMediatorHub(SRV_NAME_MEDIATOR, pMedObj);
    theMediatorHub.InitializeHub();
    pMedObj->Init();
);
