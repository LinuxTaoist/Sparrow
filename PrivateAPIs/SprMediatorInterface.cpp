/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorInterface.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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
#include "CoreTypeDefs.h"
#include "BindInterface.h"
#include "SprMediatorInterface.h"

using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) printf("%d IMediator D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%d IMediator W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d IMediator E: " fmt, __LINE__, ##args)

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

    pReqParcel->WriteInt(PROXY_CMD_GET_ALL_MQ_ATTRS);
    pReqParcel->Post();

    int ret = 0;
    pRspParcel->Wait();
    pRspParcel->ReadInt(ret);
    if (ret == 0) {
        pRspParcel->ReadVector(mqAttrVec);
    }

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}
