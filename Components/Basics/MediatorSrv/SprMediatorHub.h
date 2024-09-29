/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorHub.h
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
#ifndef __SPR_MEDIATOR_HUB_H__
#define __SPR_MEDIATOR_HUB_H__

#include <string>
#include "SprMediator.h"
#include "SprBinderHub.h"

class SprMediatorHub : public SprBinderHub
{
public:
    SprMediatorHub(const std::string& srvName, SprMediator* mediator);
    virtual ~SprMediatorHub();

    void handleCmd(const std::shared_ptr<Parcel>& pReqParcel, const std::shared_ptr<Parcel>& pRspParcel, int cmd) override;

private:
    SprMediator* mSprMediator;
};

#endif // __SPR_MEDIATOR_HUB_H__