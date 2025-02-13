/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BindInterface.cpp
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
 *
 */
#include "Parcel.h"
#include "BindCommon.h"
#include "BindInterface.h"

using namespace InternalDefs;

Parcel iReqParcel("IBinderM", KEY_IBINDER_MANAGER, true);
Parcel iRspParcel("BinderM",  KEY_BINDER_MANAGER,  false);

BindInterface* BindInterface::GetInstance()
{
    static BindInterface instance;
    return &instance;
}

bool BindInterface::InitializeServiceBinder(const std::string& srvName,
     std::shared_ptr<Parcel>& pReqParcel, std::shared_ptr<Parcel>& pRspParcel)
{
    std::shared_ptr<Binder> pBinder = AddService(srvName);
    if (!pBinder) {
        return false;
    }

    int ret = pBinder->GetParcel(pReqParcel, pRspParcel);
    if (ret != 0 || !pReqParcel || !pRspParcel) {
        return false;
    }

    return true;
}

bool BindInterface::InitializeClientBinder(const std::string& srvName,
        std::shared_ptr<Parcel>& pReqParcel, std::shared_ptr<Parcel>& pRspParcel)
{
    std::shared_ptr<IBinder> pBinder = GetService(srvName);
    if (pBinder == nullptr) {
        return false;
    }

    pBinder->GetParcel(pReqParcel, pRspParcel);
    if (pReqParcel == nullptr || pRspParcel == nullptr) {
        return false;
    }

    return true;
}

std::shared_ptr<Binder> BindInterface::AddService(const std::string& name)
{
    iReqParcel.WriteInt(BINDER_CMD_ADD_SERVICE);
    iReqParcel.WriteString(name);
    iReqParcel.Post();

    int key = 0;
    int ret = 0;
    iRspParcel.Wait();
    iRspParcel.ReadInt(key);
    iRspParcel.ReadInt(ret);

    if (ret == 0) {
        return std::make_shared<Binder>(name, key);
    } else {
        return nullptr;
    }
}

std::shared_ptr<IBinder> BindInterface::GetService(const std::string& name)
{
    iReqParcel.WriteInt(BINDER_CMD_GET_SERVICE);
    iReqParcel.WriteString(name);
    iReqParcel.Post();

    int key = 0;
    int ret = 0;
    std::string shmName;
    iRspParcel.Wait();
    iRspParcel.ReadString(shmName);
    iRspParcel.ReadInt(key);
    iRspParcel.ReadInt(ret);

    if (ret == 0) {
        return std::make_shared<IBinder>(name, key);
    } else {
        return nullptr;
    }
}

int32_t BindInterface::RemoveService(const std::string& name)
{
    iReqParcel.WriteInt(BINDER_CMD_REMOVE_SERVICE);
    iReqParcel.WriteString(name);
    iReqParcel.Post();

    int ret = 0;
    iRspParcel.Wait();
    iRspParcel.ReadInt(ret);

    return ret;
}
