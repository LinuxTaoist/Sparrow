/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BinderManager.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/03/16
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/16 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/20 | 1.0.0.2   | Xiang.D        | Rename from BindingHub to BinderManager
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __BINDER_MANAGER_H__
#define __BINDER_MANAGER_H__

#include <map>
#include <string>
#include <stdint.h>
#include "BindCommon.h"

class BinderManager
{
public:
    ~BinderManager();

    static BinderManager* GetInstance();
    int32_t HandleMsgLoop();

private:
    BinderManager();

    int32_t EnvReady(const std::string& srvName);
    int32_t BMsgRespondAddService();
    int32_t BMsgRespondRemoveService();
    int32_t BMsgRespondGetService();

private:
    using HandleFunction = int32_t (BinderManager::*)(void);

    std::map<std::string, BinderInfo> mBinderMap;
    std::map<int32_t, HandleFunction> mHandleFuncs;
};

#endif // __BINDER_MANAGER_H__
