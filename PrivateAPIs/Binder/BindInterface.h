/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BindInterface.h
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
#ifndef __BIND_INTERFACE_H__
#define __BIND_INTERFACE_H__

#include <string>
#include <memory>
#include "Binder.h"
#include "IBinder.h"

class BindInterface
{
public:
    ~BindInterface() = default;

    static BindInterface* GetInstance();
    bool InitializeServiceBinder(const std::string& srvName, std::shared_ptr<Parcel>& pReqParcel, std::shared_ptr<Parcel>& pRspParcel);
    bool InitializeClientBinder(const std::string& srvName, std::shared_ptr<Parcel>& pReqParcel, std::shared_ptr<Parcel>& pRspParcel);

private:
    BindInterface() = default;
    std::shared_ptr<Binder>  AddService(const std::string& name);
    std::shared_ptr<IBinder> GetService(const std::string& name);
    int32_t RemoveService(const std::string& name);
};

#endif // __BIND_INTERFACE_H__
