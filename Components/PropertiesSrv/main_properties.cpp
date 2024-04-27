/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_properties.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/03/13
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/13 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
#include "BindInterface.h"
#include "CoreTypeDefs.h"
#include "PropertyManager.h"

using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("Properties", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("Properties", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("Properties", fmt, ##args)

static void BinderLoop(PropertyManager* pProperM)
{
    std::shared_ptr<Parcel> pReqParcel = nullptr;
    std::shared_ptr<Parcel> pRspParcel = nullptr;
    bool ret = BindInterface::GetInstance()->InitializeServiceBinder("property_service", pReqParcel, pRspParcel);
    if (!ret)
    {
        SPR_LOGE("InitializeServiceBinder failed!\n");
        return;
    }

    int cmd = 0;
    do {
        pReqParcel->Wait();
        int ret = pReqParcel->ReadInt(cmd);
        if (ret != 0)
        {
            SPR_LOGE("ReadInt failed!\n");
            continue;
        }

        switch (cmd)
        {
            case PROPERTY_CMD_SET_PROPERTY:
            {
                std::string key;
                std::string value;
                pReqParcel->ReadString(key);
                pReqParcel->ReadString(value);
                ret = pProperM->SetProperty(key, value);

                pRspParcel->WriteInt(ret);
                pRspParcel->Post();
                break;
            }

            case PROPERTY_CMD_GET_PROPERTY:
            {
                std::string key;
                std::string value;
                std::string defaultValue;
                pReqParcel->ReadString(key);
                pReqParcel->ReadString(defaultValue);
                ret = pProperM->GetProperty(key, value, defaultValue);

                pRspParcel->WriteString(value);
                pRspParcel->WriteInt(ret);
                pRspParcel->Post();
                break;
            }

            case PROPERTY_CMD_GET_PROPERTIES:
            {
                pProperM->GetProperties();
                pRspParcel->WriteInt(0);
                pRspParcel->Post();
                break;
            }

            default:
            {
                SPR_LOGE("Unknown cmd: 0x%x\n", cmd);
                break;
            }
        }
    } while(1);
}

int main(int argc, char * argv[])
{
    PropertyManager* pProperM = PropertyManager::GetInstance();
    pProperM->Init();
    BinderLoop(pProperM);

    return 0;
}
