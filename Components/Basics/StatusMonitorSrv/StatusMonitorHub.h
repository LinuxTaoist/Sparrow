/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : StatusMonitorHub.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/10/12
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __STATUS_MONITOR_HUB_H__
#define __STATUS_MONITOR_HUB_H__

#include <string>
#include "StatusMonitorManager.h"
#include "BinderHub.h"

class StatusMonitorHub : public BinderHub {
public:
    StatusMonitorHub(const std::string& srvName, StatusMonitorManager* statusMonitorManager);
    virtual ~StatusMonitorHub();

    void handleCmd(const std::shared_ptr<Parcel>& pReqParcel, const std::shared_ptr<Parcel>& pRspParcel, int cmd) override;

private:
    StatusMonitorManager* mStatusMonitorManager;
};

#endif // __STATUS_MONITOR_HUB_H__
