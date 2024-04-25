/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IBinderManager.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/03/16
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/16 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __IBINDER_MANAGER_H__
#define __IBINDER_MANAGER_H__

#include <string>
#include <memory>
#include "Binder.h"
#include "IBinder.h"

class IBinderManager
{
public:
    ~IBinderManager() = default;

    static IBinderManager* GetInstance();
    std::shared_ptr<Binder> AddService(const std::string& name);
    std::shared_ptr<IBinder> GetService(const std::string& name);
    int32_t RemoveService(const std::string& name);
    bool InitializeServiceBinder(const std::string& srvName, std::shared_ptr<Parcel>& pReqParcel, std::shared_ptr<Parcel>& pRspParcel);
    bool InitializeClientBinder(const std::string& srvName, std::shared_ptr<Parcel>& pReqParcel, std::shared_ptr<Parcel>& pRspParcel);

private:
    IBinderManager() = default;
};

#endif // __IBINDER_MANAGER_H__
