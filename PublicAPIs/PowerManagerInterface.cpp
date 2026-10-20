/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PowerManagerInterface.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/11
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <mutex>
#include <atomic>
#include <memory>
#include <stdio.h>
#include <unistd.h>
#include "Parcel.h"
#include "AsyncEvent.h"
#include "ProcMutex.h"
#include "CoreTypeDefs.h"
#include "CommonMacros.h"
#include "BindInterface.h"
#include "GeneralUtils.h"
#include "PowerManagerInterface.h"

using namespace InternalDefs;
using namespace GeneralUtils;

#define SPR_LOGI(fmt, args...) printf("%s %6d %12s I: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IPowerMgr", __LINE__, ##args)
#define SPR_LOGD(fmt, args...) printf("%s %6d %12s D: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IPowerMgr", __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%s %6d %12s W: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IPowerMgr", __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%s %6d %12s E: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IPowerMgr", __LINE__, ##args)

static bool mEnable;
static std::atomic<bool> gObjAlive(true);
static std::mutex gTMutex;
static ProcMutex gPMutex("IPowerMgrMutex");
static std::shared_ptr<Parcel> pReqParcel = nullptr;
static std::shared_ptr<Parcel> pRspParcel = nullptr;

PowerManagerInterface::PowerManagerInterface() {
    mEnable = true;
    bool ret = BindInterface::GetInstance()->InitializeClientBinder(SRV_NAME_POWER_MANAGER, pReqParcel, pRspParcel);
    if (!ret || !pReqParcel || !pRspParcel) {
        mEnable = false;
    }
}

PowerManagerInterface::~PowerManagerInterface() {
    mEnable = false;
    gObjAlive = false;
}

PowerManagerInterface* PowerManagerInterface::GetInstance() {
    if (!gObjAlive) {
        return nullptr;
    }

    static PowerManagerInterface instance;
    return &instance;
}

int PowerManagerInterface::PowerOn() {
    if (!mEnable) {
        SPR_LOGE("PowerManager is disable!\n");
        return -1;
    }

    ProcLockGuard lock(gPMutex, gTMutex);
    NONZERO_CHECK_RET(pReqParcel->WriteInt(POWERM_CMD_POWER_ON));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int PowerManagerInterface::PowerOff() {
    if (!mEnable) {
        SPR_LOGE("PowerManager is disable!\n");
        return -1;
    }

    ProcLockGuard lock(gPMutex, gTMutex);
    NONZERO_CHECK_RET(pReqParcel->WriteInt(POWERM_CMD_POWER_OFF));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int PowerManagerInterface::RegisterCallback(void (*callback)(int32_t eventID, void* data, int32_t size)) {
    if (!mEnable) {
        SPR_LOGE("PowerManager is disable!\n");
        return -1;
    }

    ProcLockGuard lock(gPMutex, gTMutex);
    int ret = AsyncEvent::GetInstance()->RegisterEventCallback(
              SRV_NAME_POWER_MANAGER, pReqParcel, pRspParcel, callback);
    SPR_LOGD("ret: %d\n", ret);
    return ret;
}


int PowerManagerInterface::UnRegisterCallback() {
    if (!mEnable) {
        SPR_LOGE("PowerManager is disable!\n");
        return -1;
    }

    ProcLockGuard lock(gPMutex, gTMutex);
    int ret = AsyncEvent::GetInstance()->UnregisterEventCallback(
        SRV_NAME_POWER_MANAGER, pReqParcel, pRspParcel);
    SPR_LOGD("ret: %d\n", ret);
    return ret;
}
