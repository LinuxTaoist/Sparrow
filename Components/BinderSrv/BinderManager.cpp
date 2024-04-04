/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BinderManager.cpp
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
#include "Parcel.h"
#include "Shared.h"
#include "DefineMacro.h"
#include "BinderManager.h"

using namespace InternalEnum;

#define SPR_LOGD(fmt, args...) LOGD("BinderM", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGD("BinderM", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("BinderM", fmt, ##args)

const int INT_KEY_LENGTH = 5;

Parcel reqParcel("IBinderM", 66666, false);
Parcel rspParcel("BinderM",  88888, true);

BinderManager::BinderManager()
{
    mRunning = false;

    mHandleFuncs.insert(std::make_pair((int32_t)BINDER_CMD_ADD_SERVICE,     &BinderManager::MsgResponseAddService));
    mHandleFuncs.insert(std::make_pair((int32_t)BINDER_CMD_REMOVE_SERVICE,  &BinderManager::MsgResponseRemoveService));
    mHandleFuncs.insert(std::make_pair((int32_t)BINDER_CMD_GET_SERVICE,     &BinderManager::MsgResponseGetService));
}

BinderManager::~BinderManager()
{
}

BinderManager* BinderManager::GetInstance()
{
    static BinderManager instance;
    return &instance;
}

int32_t BinderManager::MsgResponseAddService()
{
    std::string name;
    int32_t key = Shared::ProduceRandomInt(INT_KEY_LENGTH);
    reqParcel.ReadString(name);

    if (mBinderMap.count(name) == 0) {
        mBinderMap.insert(make_pair(name, BinderInfo(key, name)));
    } else {
        SPR_LOGE("Service %s has exist!\n", name.c_str());
    }

    rspParcel.WriteInt(key);
    rspParcel.WriteInt(0);
    rspParcel.Post();
    SPR_LOGD("Add service info(%d, %s) \n", key, name.c_str());
    return 0;
}

int32_t BinderManager::MsgResponseRemoveService()
{
    std::string name;
    reqParcel.ReadString(name);
    mBinderMap.erase(name);

    rspParcel.WriteInt(0);
    rspParcel.Post();
    SPR_LOGD("Remove service %s \n", name.c_str());
    return 0;
}

int32_t BinderManager::MsgResponseGetService()
{
    int32_t rspRet = 0;
    int32_t key = 0;
    std::string name;
    std::string shmName;
    reqParcel.ReadString(name);

    auto it = mBinderMap.find(name);
    if (it != mBinderMap.end()) {
        shmName = it->second.GetShmName();
        key = it->second.GetKey();
    } else {
        rspRet = -1;
        SPR_LOGE("Service %s not exist!\n", name.c_str());
    }

    rspParcel.WriteString(shmName);
    rspParcel.WriteInt(key);
    rspParcel.WriteInt(rspRet);
    rspParcel.Post();

    return 0;
}

int32_t BinderManager::HandleMsgLoop()
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

    return 0;
}
