/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_time.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Global message timers and system time synchronization service.
 *  @date       : 2026/11/09
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <memory>
#include "SprLog.h"
#include "CoreTypeDefs.h"
#include "SprTimeTrace.h"
#include "SprSystemTimer.h"
#include "SprTimerManager.h"
#include "TimeManager.h"
#include "SprMainInterface.h"

using namespace InternalDefs;

#define LOG_TAG "MainTime"
#define TTP(ID, TEXT) SprTimeTrace::GetInstance()->TimeTracePoint(ID, TEXT)

SPR_ENTRY(
    SPR_LOGI("Main start!\n");

    TTP(8, "pSystemTimer->Initialize()");
    std::shared_ptr<SprSystemTimer> pSystemTimer = std::make_shared<SprSystemTimer>(MODULE_SYSTEM_TIMER, "SysTimer");
    pSystemTimer->Initialize();

    TTP(9, "TimerManager->Initialize()");
    SprTimerManager::GetInstance(MODULE_TIMERM, "TimerM", pSystemTimer)->Initialize();

    TTP(10, "TimeManager->Initialize()");
    TimeManager::GetInstance(MODULE_TIMEM, "TimeM")->Initialize();
);
