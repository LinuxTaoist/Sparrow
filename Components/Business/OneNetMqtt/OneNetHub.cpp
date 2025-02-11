/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetHub.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/10/24
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/10/24 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "SprLog.h"
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
#include "OneNetHub.h"

#define SPR_LOGD(fmt, args...) LOGD("OneNetHub", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGD("OneNetHub", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("OneNetHub", fmt, ##args)

using namespace InternalDefs;
OneNetHub::OneNetHub(const std::string& srvName, OneNetManager* oneMgr) : SprBinderHub(srvName)
{
    mOneMgr = oneMgr;
}

OneNetHub::~OneNetHub()
{
}

void OneNetHub::handleCmd(const std::shared_ptr<Parcel>& pReqParcel, const std::shared_ptr<Parcel>& pRspParcel, int cmd)
{
    switch(cmd) {
        case ONENET_CMD_ACTIVE_DEVICE: {
            std::string deviceName;
            NONZERO_CHECK(pReqParcel->ReadString(deviceName));

            SprMsg msg(SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT);
            msg.SetString(deviceName);
            mOneMgr->SendMsg(msg);
            NONZERO_CHECK(pRspParcel->WriteInt(0));
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        case ONENET_CMD_DEACTIVE_DEVICE : {
            SprMsg msg(SIG_ID_ONENET_MGR_DEACTIVE_DEVICE_DISCONNECT);
            mOneMgr->SendMsg(msg);

            NONZERO_CHECK(pRspParcel->WriteInt(0));
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        default: {
            SPR_LOGE("Unknown cmd: 0x%x\n", cmd);
            break;
        }
    }
}
