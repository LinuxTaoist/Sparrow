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
#include "SprCommonType.h"
#include "IBinderManager.h"
#include "SprCommonType.h"
#include "PropertyManager.h"

using namespace InternalEnum;

#define SPR_LOGD(fmt, args...)  printf("%d Properties D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...)  printf("%d Properties D: " fmt, __LINE__, ##args)

static void BinderLoop(PropertyManager* pProperM)
{
    IBinderManager* pInstance = IBinderManager::GetInstance();
    std::shared_ptr<Binder> pBinder = pInstance->AddService("property_service");

    std::shared_ptr<Parcel> pReqParcel = nullptr;
    std::shared_ptr<Parcel> pRspParcel = nullptr;
    pBinder->GetParcel(pReqParcel, pRspParcel);
    if (pReqParcel == nullptr || pRspParcel == nullptr) {
        SPR_LOGE("GetParcel failed!\n");
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
                int ret = pProperM->SetProperty(key, value);

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
                int ret = pProperM->GetProperty(key, value, defaultValue);

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
