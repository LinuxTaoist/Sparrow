/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PowerManagerHub.h
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
#ifndef __POWER_MANAGER_HUB_H__
#define __POWER_MANAGER_HUB_H__

#include <string>
#include "PowerManager.h"
#include "SprBinderHub.h"

class PowerManagerHub : public SprBinderHub
{
public:
    PowerManagerHub(const std::string& srvName, PowerManager* powerManager);
    ~PowerManagerHub();

    void handleCmd(std::shared_ptr<Parcel> pReqParcel, std::shared_ptr<Parcel> pRspParcel, int cmd);

private:
    PowerManager* mPowerManager;
};

#endif // __POWER_MANAGER_HUB_H__
