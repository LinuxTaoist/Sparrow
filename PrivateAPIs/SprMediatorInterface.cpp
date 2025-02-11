/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorInterface.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/04/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/04/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
#include "GeneralUtils.h"
#include "BindInterface.h"
#include "SprMediatorInterface.h"

using namespace InternalDefs;
using namespace GeneralUtils;

#define SPR_LOGI(fmt, args...) printf("%s %6d %12s I: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IMediator", __LINE__, ##args)
#define SPR_LOGD(fmt, args...) printf("%s %6d %12s D: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IMediator", __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%s %6d %12s W: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IMediator", __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%s %6d %12s E: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IMediator", __LINE__, ##args)

static bool mEnable;
std::shared_ptr<Parcel> pReqParcel = nullptr;
std::shared_ptr<Parcel> pRspParcel = nullptr;

SprMediatorInterface::SprMediatorInterface()
{
    mEnable = true;
    bool ret = BindInterface::GetInstance()->InitializeClientBinder("mediatorsrv", pReqParcel, pRspParcel);
    if (!ret || !pReqParcel || !pRspParcel) {
        mEnable = false;
    }
}

SprMediatorInterface::~SprMediatorInterface()
{
}

SprMediatorInterface* SprMediatorInterface::GetInstance()
{
    static SprMediatorInterface instance;
    return &instance;
}

int SprMediatorInterface::GetAllMQStatus(std::vector<SMQStatus>& mqAttrVec)
{
    if (!mEnable) {
        SPR_LOGE("Property is disable!\n");
        return -1;
    }

    NONZERO_CHECK_RET(pReqParcel->WriteInt(PROXY_CMD_GET_ALL_MQ_ATTRS));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));
    if (ret == 0) {
        NONZERO_CHECK_RET(pRspParcel->ReadVector(mqAttrVec));
    }

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

std::string SprMediatorInterface::GetSigalName(int sig)
{
    if (!mEnable) {
        SPR_LOGE("Property is disable!\n");
        return "";
    }

    NONZERO_CHECK_ERR(pReqParcel->WriteInt(PROXY_CMD_GET_SIGNAL_NAME), "");
    NONZERO_CHECK_ERR(pReqParcel->WriteInt(sig), "");
    NONZERO_CHECK_ERR(pReqParcel->Post(), "");

    std::string name;
    NONZERO_CHECK_ERR(pRspParcel->TimedWait(), "");
    NONZERO_CHECK_ERR(pRspParcel->ReadString(name), "");
    return name;
}
