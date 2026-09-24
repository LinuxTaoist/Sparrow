/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PropertyManagerHub.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/03/13
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __PROPERTY_MANAGER_HUB_H__
#define __PROPERTY_MANAGER_HUB_H__

#include <string>
#include "PropertyManager.h"
#include "BinderHub.h"

class PropertyManagerHub : public BinderHub {
public:
    PropertyManagerHub(const std::string& srvName, PropertyManager* powerManager);
    virtual ~PropertyManagerHub();

    void handleCmd(const std::shared_ptr<Parcel>& pReqParcel, const std::shared_ptr<Parcel>& pRspParcel, int cmd);

private:
    PropertyManager* mPropertyManager;
};

#endif // __PROPERTY_MANAGER_HUB_H__
