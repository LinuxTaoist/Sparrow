/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Property.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
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
#include <unistd.h>
#include <sys/types.h>
#include "Property.h"
#include "CoreTypeDefs.h"
#include "GeneralUtils.h"
#include "BindInterface.h"

using namespace InternalDefs;
using namespace GeneralUtils;

#define SPR_LOGI(fmt, args...) printf("%s %6d %12s I: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "Property", __LINE__, ##args)
#define SPR_LOGD(fmt, args...) printf("%s %6d %12s D: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "Property", __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%s %6d %12s W: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "Property", __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%s %6d %12s E: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "Property", __LINE__, ##args)

static bool mEnable;
std::shared_ptr<Parcel> pReqParcel = nullptr;
std::shared_ptr<Parcel> pRspParcel = nullptr;

Property::Property()
{
    mEnable = true;
    bool ret = BindInterface::GetInstance()->InitializeClientBinder("property_service", pReqParcel, pRspParcel);
    if (!ret || !pReqParcel || !pRspParcel) {
        mEnable = false;
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
    if (!mEnable) {
        SPR_LOGE("Property is disable!\n");
        return -1;
    }

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
    if (!mEnable) {
        SPR_LOGE("Property is disable!\n");
        return -1;
    }

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
    if (!mEnable) {
        SPR_LOGE("Property is disable!\n");
        return -1;
    }

    pReqParcel->WriteInt(PROPERTY_CMD_GET_PROPERTIES);
    pReqParcel->Post();

    int ret = 0;
    pRspParcel->Wait();
    pRspParcel->ReadInt(ret);
    SPR_LOGD("ret: %d\n", ret);
    return ret;
}
