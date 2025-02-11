/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PropertyManagerHub.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/17
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/17 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "SprLog.h"
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
#include "PropertyManagerHub.h"

#define SPR_LOGD(fmt, args...) LOGD("PropertyManager", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGD("PropertyManager", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("PropertyManager", fmt, ##args)

using namespace InternalDefs;
PropertyManagerHub::PropertyManagerHub(const std::string& srvName, PropertyManager* powerManager) : SprBinderHub(srvName)
{
    mPropertyManager = powerManager;
}

PropertyManagerHub::~PropertyManagerHub()
{
}

void PropertyManagerHub::handleCmd(const std::shared_ptr<Parcel>& pReqParcel, const std::shared_ptr<Parcel>& pRspParcel, int cmd)
{
    switch (cmd) {
        case PROPERTY_CMD_SET_PROPERTY: {
            std::string key;
            std::string value;
            NONZERO_CHECK(pReqParcel->ReadString(key));
            NONZERO_CHECK(pReqParcel->ReadString(value));
            int ret = mPropertyManager->SetProperty(key, value);

            NONZERO_CHECK(pRspParcel->WriteInt(ret));
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        case PROPERTY_CMD_GET_PROPERTY: {
            std::string key;
            std::string value;
            std::string defaultValue;
            NONZERO_CHECK(pReqParcel->ReadString(key));
            NONZERO_CHECK(pReqParcel->ReadString(defaultValue));
            int ret = mPropertyManager->GetProperty(key, value, defaultValue);

            NONZERO_CHECK(pRspParcel->WriteString(value));
            NONZERO_CHECK(pRspParcel->WriteInt(ret));
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        case PROPERTY_CMD_GET_PROPERTIES: {
            mPropertyManager->GetProperties();
            NONZERO_CHECK(pRspParcel->WriteInt(0));
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        default: {
            SPR_LOGE("Unknown cmd: 0x%x\n", cmd);
            break;
        }
    }
}
