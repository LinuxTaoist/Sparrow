/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BinderManager.cpp
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
 */
#include <fcntl.h>
#include <unistd.h>
#include "Parcel.h"
#include "SprLog.h"
#include "CommonMacros.h"
#include "GeneralUtils.h"
#include "BindCommon.h"
#include "BinderManager.h"

using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("BinderM", fmt, ##args)
#define SPR_LOGI(fmt, args...) LOGD("BinderM", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGD("BinderM", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("BinderM", fmt, ##args)

#define INT_KEY_LENGTH  5

Parcel reqParcel("IBinderM", KEY_IBINDER_MANAGER, false);
Parcel rspParcel("BinderM",  KEY_BINDER_MANAGER,  true);

bool BinderManager::mRunning = false;

BinderManager::BinderManager()
{
    mHandleFuncs.insert(std::make_pair((int32_t)BINDER_CMD_ADD_SERVICE,     &BinderManager::BMsgRespondAddService));
    mHandleFuncs.insert(std::make_pair((int32_t)BINDER_CMD_REMOVE_SERVICE,  &BinderManager::BMsgRespondRemoveService));
    mHandleFuncs.insert(std::make_pair((int32_t)BINDER_CMD_GET_SERVICE,     &BinderManager::BMsgRespondGetService));
    EnvReady(SRV_NAME_BINDER);
}

BinderManager::~BinderManager()
{
}

BinderManager* BinderManager::GetInstance()
{
    static BinderManager instance;
    return &instance;
}

int32_t BinderManager::EnvReady(const std::string& srvName)
{
    std::string node = "/tmp/" + srvName;
    int fd = creat(node.c_str(), 0644);
    if (fd != -1) {
        close(fd);
    }

    return 0;
}

int32_t BinderManager::BMsgRespondAddService()
{
    std::string name;
    int32_t key = GeneralUtils::GetRandomInteger(INT_KEY_LENGTH);
    reqParcel.ReadString(name);

    mBinderMap[name] = BinderInfo(key, name);

    rspParcel.WriteInt(key);
    rspParcel.WriteInt(0);
    rspParcel.Post();
    SPR_LOGD("Add service info(%d, %s) \n", key, name.c_str());
    return 0;
}

int32_t BinderManager::BMsgRespondRemoveService()
{
    std::string name;
    reqParcel.ReadString(name);
    mBinderMap.erase(name);

    rspParcel.WriteInt(0);
    rspParcel.Post();
    SPR_LOGD("Remove service %s \n", name.c_str());
    return 0;
}

int32_t BinderManager::BMsgRespondGetService()
{
    int32_t ret = 0;
    int32_t key = 0;
    std::string name;
    std::string shmName;
    reqParcel.ReadString(name);

    auto it = mBinderMap.find(name);
    if (it != mBinderMap.end()) {
        shmName = it->second.GetShmName();
        key = it->second.GetKey();
    } else {
        ret = -1;
        SPR_LOGE("Service %s not exist!\n", name.c_str());
    }

    rspParcel.WriteString(shmName);
    rspParcel.WriteInt(key);
    rspParcel.WriteInt(ret);
    rspParcel.Post();

    return ret;
}

int32_t BinderManager::StartWork()
{
    mRunning = true;
    while (mRunning)
    {
        int cmd = 0;
        reqParcel.Wait();
        reqParcel.ReadInt(cmd);

        auto handler = mHandleFuncs.find(cmd);
        if (handler != mHandleFuncs.end()) {
            HandleFunction handleFunc = handler->second;
            (this->*handleFunc)();
        } else {
            SPR_LOGE("No cmd 0x%x!\n", cmd);
        }
    }

    SPR_LOGI("Exit work loop!\n");
    return 0;
}

int32_t BinderManager::StopWork()
{
    mRunning = false;

    // Signal to unblock the reqParcel.Wait() call
    reqParcel.WriteInt(0);
    reqParcel.Post();
    SPR_LOGI("Stop work!\n");
    return 0;
}
