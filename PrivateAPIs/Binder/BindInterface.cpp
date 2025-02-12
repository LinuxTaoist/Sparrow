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
#include <atomic>
#include "Parcel.h"
#include "BindCommon.h"
#include "BindInterface.h"
#include "CommonMacros.h"

using namespace InternalDefs;

static std::atomic<bool> gObjAlive(true);
Parcel iReqParcel("IBinderM", KEY_IBINDER_MANAGER, false);
Parcel iRspParcel("BinderM",  KEY_BINDER_MANAGER,  false);

BindInterface::~BindInterface()
{
    gObjAlive = false;
}

BindInterface* BindInterface::GetInstance()
{
    if (!gObjAlive) {
        return nullptr;
    }

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
    NONZERO_CHECK_ERR(iReqParcel.WriteInt(BINDER_CMD_ADD_SERVICE), nullptr);
    NONZERO_CHECK_ERR(iReqParcel.WriteString(name), nullptr);
    NONZERO_CHECK_ERR(iReqParcel.Post(), nullptr);

    int key = 0;
    int ret = 0;
    NONZERO_CHECK_ERR(iRspParcel.TimedWait(), nullptr);
    NONZERO_CHECK_ERR(iRspParcel.ReadInt(key), nullptr);
    NONZERO_CHECK_ERR(iRspParcel.ReadInt(ret), nullptr);

    if (ret == 0) {
        return std::make_shared<Binder>(name, key);
    } else {
        return nullptr;
    }
}

std::shared_ptr<IBinder> BindInterface::GetService(const std::string& name)
{
    NONZERO_CHECK_ERR(iReqParcel.WriteInt(BINDER_CMD_GET_SERVICE), nullptr);
    NONZERO_CHECK_ERR(iReqParcel.WriteString(name), nullptr);
    NONZERO_CHECK_ERR(iReqParcel.Post(), nullptr);

    int key = 0;
    int ret = 0;
    std::string shmName;
    NONZERO_CHECK_ERR(iRspParcel.TimedWait(), nullptr);
    NONZERO_CHECK_ERR(iRspParcel.ReadString(shmName), nullptr);
    NONZERO_CHECK_ERR(iRspParcel.ReadInt(key), nullptr);
    NONZERO_CHECK_ERR(iRspParcel.ReadInt(ret), nullptr);

    if (ret == 0) {
        return std::make_shared<IBinder>(name, key);
    } else {
        return nullptr;
    }
}

int32_t BindInterface::RemoveService(const std::string& name)
{
    NONZERO_CHECK_RET(iReqParcel.WriteInt(BINDER_CMD_REMOVE_SERVICE));
    NONZERO_CHECK_RET(iReqParcel.WriteString(name));
    NONZERO_CHECK_RET(iReqParcel.Post());

    int ret = 0;
    NONZERO_CHECK_RET(iRspParcel.TimedWait());
    NONZERO_CHECK_RET(iRspParcel.ReadInt(ret));

    return ret;
}
