/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetHub.h
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
#ifndef __ONENET_HUB_H__
#define __ONENET_HUB_H__

#include "OneNetManager.h"
#include "SprBinderHub.h"

class OneNetHub : public SprBinderHub {
public:
    OneNetHub(const std::string& srvName, OneNetManager* oneMgr);
    virtual ~OneNetHub();

    void handleCmd(const std::shared_ptr<Parcel>& pReqParcel, const std::shared_ptr<Parcel>& pRspParcel, int cmd) override;

private:
    OneNetManager* mOneMgr;
};

#endif // __ONENET_HUB_H__
