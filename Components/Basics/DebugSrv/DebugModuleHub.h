/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DebugModuleHub.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/28
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/28 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __DEBUG_MODULE_HUB_H__
#define __DEBUG_MODULE_HUB_H__

#include <string>
#include "DebugModule.h"
#include "SprBinderHub.h"

class DebugModuleHub : public SprBinderHub
{
public:
    DebugModuleHub(const std::string& srvName, DebugModule* pDebugModule);
    ~DebugModuleHub();

    void handleCmd(const std::shared_ptr<Parcel>& pReqParcel, const std::shared_ptr<Parcel>& pRspParcel, int cmd);

private:
    DebugModule* mDebugModulePtr;
};

#endif // __DEBUG_MODULE_HUB_H__
