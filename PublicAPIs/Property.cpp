/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Property.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/04/01
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/04/01 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <memory.h>
#include "Property.h"
#include "CoreTypeDefs.h"
#include "IBinderManager.h"

using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) printf("%d Property D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%d Property W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d Property E: " fmt, __LINE__, ##args)

IBinderManager* pObj = nullptr;
std::shared_ptr<IBinder> pBinder = nullptr;
std::shared_ptr<Parcel> pReqParcel = nullptr;
std::shared_ptr<Parcel> pRspParcel = nullptr;

Property::Property()
{
    pObj = IBinderManager::GetInstance();
    if (pObj == nullptr) {
        SPR_LOGE("GetInstance failed\n");
        exit(0);
    }

    pBinder = pObj->GetService("property_service");
    if (pBinder == nullptr) {
        SPR_LOGE("GetService failed\n");
        exit(0);
    }

    pBinder->GetParcel(pReqParcel, pRspParcel);
    if (pReqParcel == nullptr || pRspParcel == nullptr) {
        SPR_LOGE("GetParcel failed!\n");
        exit(0);
    }
}

Property::~Property()
{
}

Property* Property::GetInstance()
{
    static Property instance;
    return &instance;
}

int Property::SetProperty(const std::string& key, const std::string& value)
{
    pReqParcel->WriteInt(PROPERTY_CMD_SET_PROPERTY);
    pReqParcel->WriteString(key);
    pReqParcel->WriteString(value);
    pReqParcel->Post();

    int ret = 0;
    pRspParcel->Wait();
    pRspParcel->ReadInt(ret);
    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int Property::GetProperty(const std::string& key, std::string& value, const std::string& defaultValue)
{
    pReqParcel->WriteInt(PROPERTY_CMD_GET_PROPERTY);
    pReqParcel->WriteString(key);
    pReqParcel->WriteString(defaultValue);
    pReqParcel->Post();

    int ret = 0;
    pRspParcel->Wait();
    pRspParcel->ReadString(value);
    pRspParcel->ReadInt(ret);
    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int Property::GetProperties()
{
    pReqParcel->WriteInt(PROPERTY_CMD_GET_PROPERTIES);
    pReqParcel->Post();

    int ret = 0;
    pRspParcel->Wait();
    pRspParcel->ReadInt(ret);
    SPR_LOGD("ret: %d\n", ret);
    return ret;
}
