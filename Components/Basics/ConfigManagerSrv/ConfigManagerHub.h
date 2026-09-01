/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ConfigManagerHub.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/06/20
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/06/20 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __CONFIG_MANAGER_HUB_H__
#define __CONFIG_MANAGER_HUB_H__

#include <string>
#include "ConfigManager.h"
#include "BinderHub.h"

class ConfigManagerHub : public BinderHub
{
public:
    ConfigManagerHub(const std::string& srvName, ConfigManager* pManager);
    virtual ~ConfigManagerHub();

    void handleCmd(const std::shared_ptr<Parcel>& pReqParcel,
                   const std::shared_ptr<Parcel>& pRspParcel, int32_t cmd) override;

private:
    ConfigManager* mConfigManager;
};

#endif // __CONFIG_MANAGER_HUB_H__