/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DebugInterface.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/28
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/28 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <atomic>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "CoreTypeDefs.h"
#include "CommonMacros.h"
#include "GeneralUtils.h"
#include "BindInterface.h"
#include "CommonErrorCodes.h"
#include "DebugInterface.h"

using namespace InternalDefs;
using namespace GeneralUtils;

#define SPR_LOGI(fmt, args...) printf("%s %6d %12s I: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IDebug", __LINE__, ##args)
#define SPR_LOGD(fmt, args...) printf("%s %6d %12s D: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IDebug", __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%s %6d %12s W: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IDebug", __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%s %6d %12s E: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IDebug", __LINE__, ##args)

static bool mEnable;
static std::atomic<bool> gObjAlive(true);
std::shared_ptr<Parcel> pReqParcel = nullptr;
std::shared_ptr<Parcel> pRspParcel = nullptr;

DebugInterface::DebugInterface()
{
    mEnable = true;
    bool ret = BindInterface::GetInstance()->InitializeClientBinder(SRV_NAME_DEBUG_MODULE, pReqParcel, pRspParcel);
    if (!ret || !pReqParcel || !pRspParcel) {
        mEnable = false;
    }
}

DebugInterface::~DebugInterface()
{
    mEnable = false;
    gObjAlive = false;
}

DebugInterface* DebugInterface::GetInstance()
{
    if (!gObjAlive) {
        return nullptr;
    }

    static DebugInterface instance;
    return &instance;
}

int DebugInterface::AddTimerInOneSec()
{
    NONTRUE_CHECK_ERR(mEnable, ERR_BINDER_INIT_FAILED);
    POINTER_CHECK_ERR(pReqParcel, ERR_BINDER_INVALID_POINTER);
    POINTER_CHECK_ERR(pRspParcel, ERR_BINDER_INVALID_POINTER);

    NONZERO_CHECK_RET(pReqParcel->WriteInt(DEBUG_CMD_TEST_ADD_1S_TIMER));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int DebugInterface::DelTimerInOneSec()
{
    NONTRUE_CHECK_ERR(mEnable, ERR_BINDER_INIT_FAILED);
    POINTER_CHECK_ERR(pReqParcel, ERR_BINDER_INVALID_POINTER);
    POINTER_CHECK_ERR(pRspParcel, ERR_BINDER_INVALID_POINTER);

    NONZERO_CHECK_RET(pReqParcel->WriteInt(DEBUG_CMD_TEST_DEL_1S_TIMER));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int DebugInterface::AddCustomTimer(uint32_t RepeatTimes, int32_t DelayInMilliSec, int32_t IntervalInMilliSec)
{
    NONTRUE_CHECK_ERR(mEnable, ERR_BINDER_INIT_FAILED);
    POINTER_CHECK_ERR(pReqParcel, ERR_BINDER_INVALID_POINTER);
    POINTER_CHECK_ERR(pRspParcel, ERR_BINDER_INVALID_POINTER);

    NONZERO_CHECK_RET(pReqParcel->WriteInt(DEBUG_CMD_TEST_ADD_CUSTOM_TIMER));
    NONZERO_CHECK_RET(pReqParcel->WriteInt(RepeatTimes));
    NONZERO_CHECK_RET(pReqParcel->WriteInt(DelayInMilliSec));
    NONZERO_CHECK_RET(pReqParcel->WriteInt(IntervalInMilliSec));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int DebugInterface::DelCustomTimer()
{
    NONTRUE_CHECK_ERR(mEnable, ERR_BINDER_INIT_FAILED);
    POINTER_CHECK_ERR(pReqParcel, ERR_BINDER_INVALID_POINTER);
    POINTER_CHECK_ERR(pRspParcel, ERR_BINDER_INVALID_POINTER);

    NONZERO_CHECK_RET(pReqParcel->WriteInt(DEBUG_CMD_TEST_DEL_CUSTOM_TIMER));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int DebugInterface::EnableRemoteShell()
{
    NONTRUE_CHECK_ERR(mEnable, ERR_BINDER_INIT_FAILED);
    POINTER_CHECK_ERR(pReqParcel, ERR_BINDER_INVALID_POINTER);
    POINTER_CHECK_ERR(pRspParcel, ERR_BINDER_INVALID_POINTER);

    NONZERO_CHECK_RET(pReqParcel->WriteInt(DEBUG_CMD_ENABLE_REMOTE_SHELL));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int DebugInterface::DisableRemoteShell()
{
    NONTRUE_CHECK_ERR(mEnable, ERR_BINDER_INIT_FAILED);
    POINTER_CHECK_ERR(pReqParcel, ERR_BINDER_INVALID_POINTER);
    POINTER_CHECK_ERR(pRspParcel, ERR_BINDER_INVALID_POINTER);

    NONZERO_CHECK_RET(pReqParcel->WriteInt(DEBUG_CMD_DISABLE_REMOTE_SHELL));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

