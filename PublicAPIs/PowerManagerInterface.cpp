/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PowerManagerInterface.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/11
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/11 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <sys/types.h>
#include "Parcel.h"
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
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
std::shared_ptr<Parcel> pReqParcel = nullptr;
std::shared_ptr<Parcel> pRspParcel = nullptr;

PowerManagerInterface::PowerManagerInterface()
{
    mEnable = true;
    bool ret = BindInterface::GetInstance()->InitializeClientBinder("powermanagersrv", pReqParcel, pRspParcel);
    if (!ret || !pReqParcel || !pRspParcel) {
        mEnable = false;
    }
}

PowerManagerInterface::~PowerManagerInterface()
{
    mEnable = false;
}

PowerManagerInterface* PowerManagerInterface::GetInstance()
{
    static PowerManagerInterface instance;
    return &instance;
}

int PowerManagerInterface::PowerOn()
{
    if (!mEnable) {
        SPR_LOGE("PowerManager is disable!\n");
        return -1;
    }

    NONZERO_CHECK_RET(pReqParcel->WriteInt(POWERM_CMD_POWER_ON));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int PowerManagerInterface::PowerOff()
{
    if (!mEnable) {
        SPR_LOGE("PowerManager is disable!\n");
        return -1;
    }

    NONZERO_CHECK_RET(pReqParcel->WriteInt(POWERM_CMD_POWER_OFF));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

