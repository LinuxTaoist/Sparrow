/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorHub.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/09/08
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/09/08 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "SprLog.h"
#include "CoreTypeDefs.h"
#include "SprMediatorHub.h"

#define SPR_LOGD(fmt, args...) LOGD("SprMediator", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGD("SprMediator", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("SprMediator", fmt, ##args)

using namespace InternalDefs;
SprMediatorHub::SprMediatorHub(const std::string& srvName, SprMediator* SprMediator) : SprBinderHub(srvName)
{
    mSprMediator = SprMediator;
}

SprMediatorHub::~SprMediatorHub()
{
}

void SprMediatorHub::handleCmd(const std::shared_ptr<Parcel>& pReqParcel, const std::shared_ptr<Parcel>& pRspParcel, int cmd)
{
    switch(cmd)
    {
        case PROXY_CMD_GET_ALL_MQ_ATTRS:
        {
            std::vector<SMQStatus> tmpMQAttrs;
            int ret = mSprMediator->GetAllMQStatus(tmpMQAttrs);
            pRspParcel->WriteInt(ret);
            if (ret == 0) {
                pRspParcel->WriteVector(tmpMQAttrs);
            }

            pRspParcel->Post();
            break;
        }
        default:
        {
            SPR_LOGE("Unknown cmd: 0x%x\n", cmd);
            break;
        }
    }
}

