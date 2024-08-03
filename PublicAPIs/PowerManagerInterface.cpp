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
#include "Parcel.h"
#include "CoreTypeDefs.h"
#include "BindInterface.h"
#include "PowerManagerInterface.h"

using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) printf("%d IPowerManager D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%d IPowerManager W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d IPowerManager E: " fmt, __LINE__, ##args)

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

    pReqParcel->WriteInt(POWERM_CMD_POWER_ON);
    pReqParcel->Post();

    int ret = 0;
    pRspParcel->Wait();
    pRspParcel->ReadInt(ret);

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int PowerManagerInterface::PowerOff()
{
    if (!mEnable) {
        SPR_LOGE("PowerManager is disable!\n");
        return -1;
    }

    pReqParcel->WriteInt(POWERM_CMD_POWER_OFF);
    pReqParcel->Post();

    int ret = 0;
    pRspParcel->Wait();
    pRspParcel->ReadInt(ret);

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

