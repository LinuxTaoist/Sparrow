/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PowerManagerHub.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/17
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/17 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "SprLog.h"
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
#include "PowerManagerHub.h"

#define SPR_LOGD(fmt, args...) LOGD("PowerHub", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGD("PowerHub", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("PowerHub", fmt, ##args)

using namespace InternalDefs;
PowerManagerHub::PowerManagerHub(const std::string& srvName, PowerManager* powerManager) : SprBinderHub(srvName)
{
    mPowerManager = powerManager;
}

PowerManagerHub::~PowerManagerHub()
{
}

void PowerManagerHub::handleCmd(const std::shared_ptr<Parcel>& pReqParcel, const std::shared_ptr<Parcel>& pRspParcel, int cmd)
{
    switch(cmd) {
        case POWERM_CMD_POWER_ON: {
            SprMsg msg(SIG_ID_POWER_ON);
            mPowerManager->SendMsg(msg);

            NONZERO_CHECK(pRspParcel->WriteInt(0));
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        case POWERM_CMD_POWER_OFF: {
            SprMsg msg(SIG_ID_POWER_OFF);
            mPowerManager->SendMsg(msg);

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
