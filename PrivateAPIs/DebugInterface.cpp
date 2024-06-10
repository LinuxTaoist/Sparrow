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
#include <stdio.h>
#include "CoreTypeDefs.h"
#include "CommonMacros.h"
#include "BindInterface.h"
#include "DebugInterface.h"

using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) printf("%d IDebug D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%d IDebug W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d IDebug E: " fmt, __LINE__, ##args)

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
}

DebugInterface* DebugInterface::GetInstance()
{
    static DebugInterface instance;
    return &instance;
}

int DebugInterface::AddTimerInOneSec()
{
    if (!mEnable) {
        SPR_LOGE("DebugModule is disable!\n");
        return -1;
    }

    pReqParcel->WriteInt(DEBUG_CMD_TEST_ADD_1S_TIMER);
    pReqParcel->Post();

    int ret = 0;
    pRspParcel->Wait();
    pRspParcel->ReadInt(ret);

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int DebugInterface::DelTimerInOneSec()
{
    if (!mEnable) {
        SPR_LOGE("DebugModule is disable!\n");
        return -1;
    }

    pReqParcel->WriteInt(DEBUG_CMD_TEST_DEL_1S_TIMER);
    pReqParcel->Post();

    int ret = 0;
    pRspParcel->Wait();
    pRspParcel->ReadInt(ret);

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int DebugInterface::AddCustomTimer(uint32_t RepeatTimes, int32_t DelayInMilliSec, int32_t IntervalInMilliSec)
{
    if (!mEnable) {
        SPR_LOGE("DebugModule is disable!\n");
        return -1;
    }

    pReqParcel->WriteInt(DEBUG_CMD_TEST_ADD_CUSTOM_TIMER);
    pReqParcel->WriteInt(RepeatTimes);
    pReqParcel->WriteInt(DelayInMilliSec);
    pReqParcel->WriteInt(IntervalInMilliSec);
    pReqParcel->Post();

    int ret = 0;
    pRspParcel->Wait();
    pRspParcel->ReadInt(ret);

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int DebugInterface::DelCustomTimer()
{
    if (!mEnable) {
        SPR_LOGE("DebugModule is disable!\n");
        return -1;
    }

    pReqParcel->WriteInt(DEBUG_CMD_TEST_DEL_CUSTOM_TIMER);
    pReqParcel->Post();

    int ret = 0;
    pRspParcel->Wait();
    pRspParcel->ReadInt(ret);

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int DebugInterface::EnableRemoteShell()
{
    if (!mEnable) {
        SPR_LOGE("DebugModule is disable!\n");
        return -1;
    }

    pReqParcel->WriteInt(DEBUG_CMD_ENABLE_REMOTE_SHELL);
    pReqParcel->Post();

    int ret = 0;
    pRspParcel->Wait();
    pRspParcel->ReadInt(ret);

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int DebugInterface::DisableRemoteShell()
{
    if (!mEnable) {
        SPR_LOGE("DebugModule is disable!\n");
        return -1;
    }

    pReqParcel->WriteInt(DEBUG_CMD_DISABLE_REMOTE_SHELL);
    pReqParcel->Post();

    int ret = 0;
    pRspParcel->Wait();
    pRspParcel->ReadInt(ret);

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

