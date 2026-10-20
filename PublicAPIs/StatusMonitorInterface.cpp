/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : StatusMonitorInterface.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/10/12
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <atomic>
#include <memory>
#include <mutex>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "Parcel.h"
#include "ProcMutex.h"
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
#include "BindInterface.h"
#include "GeneralUtils.h"
#include "StatusMonitorInterface.h"

using namespace InternalDefs;
using namespace GeneralUtils;

#define SPR_LOGI(fmt, args...) printf("%s %6d %12s I: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IStatusMon", __LINE__, ##args)
#define SPR_LOGD(fmt, args...) printf("%s %6d %12s D: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IStatusMon", __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%s %6d %12s W: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IStatusMon", __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%s %6d %12s E: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IStatusMon", __LINE__, ##args)

static bool mEnable;
static std::atomic<bool> gObjAlive(true);
static std::mutex gTMutex;
static ProcMutex gPMutex("IStatusMonitorMutex");
static std::shared_ptr<Parcel> pReqParcel = nullptr;
static std::shared_ptr<Parcel> pRspParcel = nullptr;

StatusMonitorInterface::StatusMonitorInterface() {
    mEnable = true;
    bool ret = BindInterface::GetInstance()->InitializeClientBinder(SRV_NAME_STATUS_MONITOR, pReqParcel, pRspParcel);
    if (!ret || !pReqParcel || !pRspParcel) {
        mEnable = false;
    }
}

StatusMonitorInterface::~StatusMonitorInterface() {
    mEnable = false;
    gObjAlive = false;
}

StatusMonitorInterface* StatusMonitorInterface::GetInstance() {
    if (!gObjAlive) {
        return nullptr;
    }

    static StatusMonitorInterface instance;
    return &instance;
}

int StatusMonitorInterface::GetAllEvents(std::vector<StatusEvent>& events) {
    if (!mEnable) {
        SPR_LOGE("StatusMonitor is disable!\n");
        return -1;
    }

    ProcLockGuard lock(gPMutex, gTMutex);
    NONZERO_CHECK_RET(pReqParcel->WriteInt(STATUSMON_CMD_GET_ALL_EVENTS));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));
    if (ret != 0) {
        return ret;
    }

    int count = 0;
    NONZERO_CHECK_RET(pRspParcel->ReadInt(count));
    events.clear();
    for (int i = 0; i < count; i++) {
        StatusEvent event;
        NONZERO_CHECK_RET(pRspParcel->ReadInt(event.sModuleID));
        NONZERO_CHECK_RET(pRspParcel->ReadInt(event.sErrorCode));
        NONZERO_CHECK_RET(pRspParcel->ReadString(event.sText));

        int timeSize = sizeof(event.sTime);
        NONZERO_CHECK_RET(pRspParcel->ReadData(&event.sTime, timeSize));
        events.push_back(event);
    }

    return 0;
}

int StatusMonitorInterface::GetEvent(std::vector<StatusEvent>& events,
                                     int32_t moduleID,
                                     int32_t level,
                                     int32_t errCode) {
    if (!mEnable) {
        SPR_LOGE("StatusMonitor is disable!\n");
        return -1;
    }

    ProcLockGuard lock(gPMutex, gTMutex);
    NONZERO_CHECK_RET(pReqParcel->WriteInt(STATUSMON_CMD_GET_EVENT));
    NONZERO_CHECK_RET(pReqParcel->WriteInt(moduleID));
    NONZERO_CHECK_RET(pReqParcel->WriteInt(level));
    NONZERO_CHECK_RET(pReqParcel->WriteInt(errCode));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));
    if (ret != 0) {
        return ret;
    }

    int count = 0;
    NONZERO_CHECK_RET(pRspParcel->ReadInt(count));
    events.clear();
    for (int i = 0; i < count; i++) {
        StatusEvent event;
        NONZERO_CHECK_RET(pRspParcel->ReadInt(event.sModuleID));
        NONZERO_CHECK_RET(pRspParcel->ReadInt(event.sErrorCode));
        NONZERO_CHECK_RET(pRspParcel->ReadString(event.sText));

        int timeSize = sizeof(event.sTime);
        NONZERO_CHECK_RET(pRspParcel->ReadData(&event.sTime, timeSize));
        events.push_back(event);
    }

    return 0;
}
