/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprSystem.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/02/24
 *
 *  System initialization file
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/02/24 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdio.h>
#include "SprSystem.h"
#include "SprTimeTrace.h"
#include "SprCommonType.h"
#include "SprTimerManager.h"
#include "SprMediatorIpcProxy.h"

using namespace std;
using namespace InternalEnum;

#define SPR_LOGD(fmt, args...) printf("%d Sys D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%d Sys W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d Sys E: " fmt, __LINE__, ##args)

#define TTP(ID, TEXT) SprTimeTrace::GetInstance()->TimeTracePoint(ID, TEXT)

SprSystem::SprSystem()
{
}

SprSystem::~SprSystem()
{
}

SprSystem* SprSystem::GetInstance()
{
    static SprSystem instance;
    return &instance;
}

void SprSystem::Init()
{
    SPR_LOGD("=============================================\n");
    SPR_LOGD("=          Sprrow System Init               =\n");
    SPR_LOGD("=============================================\n");

    TTP(10, "TimerManager->Init()");
    SprTimerManager::GetInstance(MODULE_TIMERM, "TimerM", make_shared<SprMediatorIpcProxy>())->Init();

    // Permanently block waiting for message driver to trigger
    SprObserver::MainLoop();
}


