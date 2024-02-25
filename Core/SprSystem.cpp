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
#include <memory>
#include <stdio.h>
#include <sys/resource.h>
#include "SprSystem.h"
#include "SprTimeTrace.h"
#include "SprCommonType.h"
#include "SprMediatorIpcProxy.h"

#include "SprSystemTimer.h"
#include "SprTimerManager.h"

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

void SprSystem::InitEnv()
{
    // Init msg queue limit
    InitMsgQueueLimit();
}

void SprSystem::InitMsgQueueLimit()
{
    // The limit for creating message queues has to be changed, otherwise the other
    // applications can not create enough message queues.
    // Note: The values in /proc/sys/fs/mqueue/* seem to have no influence on this issue.
    // Also ulimit -n has no influence on this issue.
    struct rlimit rlim = {RLIM_INFINITY, RLIM_INFINITY};
    const int ret = getrlimit(RLIMIT_MSGQUEUE, &rlim);
    if (ret == 0)
    {
      rlim.rlim_cur = RLIM_INFINITY; //soft limit
      rlim.rlim_max = RLIM_INFINITY; //hard limit
      setrlimit(RLIMIT_MSGQUEUE, &rlim);
    }
}

void SprSystem::Init()
{
    SPR_LOGD("=============================================\n");
    SPR_LOGD("=          Sprrow System Init               =\n");
    SPR_LOGD("=============================================\n");

    InitEnv();

    TTP(9, "systemTimerPtr->Init()");
    shared_ptr<SprSystemTimer> systemTimerPtr = make_shared<SprSystemTimer>(MODULE_SYSTEM_TIMER, "SysTimer", make_shared<SprMediatorIpcProxy>());
    systemTimerPtr->Init();

    TTP(10, "TimerManager->Init()");
    SprTimerManager::GetInstance(MODULE_TIMERM, "TimerM", make_shared<SprMediatorIpcProxy>(), systemTimerPtr)->Init();

    // Permanently block waiting for message driver to trigger
    SprObserver::MainLoop();
}


