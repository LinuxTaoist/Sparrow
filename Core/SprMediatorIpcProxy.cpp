/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorProxy.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *------------------------------------------------------------------------------
 *
 */
#include "SprCommonType.h"
#include "SprMediatorIpcProxy.h"
#include "SprObserver.h"

using namespace std;

#define SPR_LOGD(fmt, args...) printf("%d IpcProxy D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d IpcProxy E: " fmt, __LINE__, ##args)

SprMediatorIpcProxy::SprMediatorIpcProxy()
{

}

SprMediatorIpcProxy::~SprMediatorIpcProxy()
{

}

int SprMediatorIpcProxy::RegisterObserver(const SprObserver& observer)
{
    int handle = observer.getMqHandle();
    ESprModuleID id = observer.getModuleId();
    string name = observer.getModuleName();

    // TODO: 将observer信息发送给远程消息中介注册
    SPR_LOGD("Register observer: [%d] [0x%x] [%s]\n", handle, (uint32_t)id, name.c_str());
    return 0;
}

int SprMediatorIpcProxy::UnregisterObserver(const SprObserver& observer)
{
    int handle = observer.getMqHandle();
    ESprModuleID id = observer.getModuleId();
    string name = observer.getModuleName();

    // TODO: 将observer信息发送给远程消息中介注销
    SPR_LOGD("Register observer: [%d] [0x%x] [%s]\n", handle, (uint32_t)id, name.c_str());
    return 0;
}

int SprMediatorIpcProxy::NotifyObserver()
{
    return 0;
}


