/**
 *---------------------------------------------------------------------------------------------------------------------
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
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <errno.h>
#include <string.h>
#include <mqueue.h>
#include "SprMsg.h"
#include "SprCommonType.h"
#include "SprObserver.h"
#include "DefineMacro.h"
#include "SprMediatorIpcProxy.h"

using namespace std;
using namespace InternalEnum;

#define SPR_LOGD(fmt, args...) LOGD("IpcProxy", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("IpcProxy", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("IpcProxy", fmt, ##args)

SprMediatorIpcProxy::SprMediatorIpcProxy()
{
    mMediatorHandle = -1;
    ConnectMediator();
}

SprMediatorIpcProxy::~SprMediatorIpcProxy()
{
    if (mMediatorHandle != -1)
    {
        mq_close(mMediatorHandle);
        mMediatorHandle = -1;
    }
}

int SprMediatorIpcProxy::ConnectMediator()
{
    mq_attr mqAttr;
    mqAttr.mq_maxmsg = 10;      // cat /proc/sys/fs/mqueue/msg_max
    mqAttr.mq_msgsize = 1025;

    mMediatorHandle = mq_open(MEDIATOR_MSG_QUEUE, O_RDWR, 0666, &mqAttr);
    if(mMediatorHandle < 0) {
        SPR_LOGE("Open %s failed. (%s)\n", MEDIATOR_MSG_QUEUE, strerror(errno));
        return -1;
    }

    return 0;
}

int SprMediatorIpcProxy::RegisterObserver(const SprObserver& observer)
{
    SprMsg msg(observer.GetModuleId(), MODULE_PROXY, SIG_ID_PROXY_REGISTER_REQUEST);
    msg.SetBoolValue(observer.IsMonitored());
    msg.SetU32Value((uint32_t)PROXY_TYPE_MQ);
    msg.SetU16Value((uint16_t)observer.GetModuleId());
    msg.SetString(observer.GetMqDevName());

    SendMsg(msg);
    SPR_LOGD("Register observer: [0x%x] [%s]\n", observer.GetModuleId(), observer.GetMqDevName().c_str());
    return 0;
}

int SprMediatorIpcProxy::UnregisterObserver(const SprObserver& observer)
{
    SprMsg msg(observer.GetModuleId(), MODULE_PROXY, SIG_ID_PROXY_UNREGISTER_REQUEST);
    msg.SetU32Value((uint32_t)PROXY_TYPE_MQ);
    msg.SetU16Value((uint16_t)observer.GetModuleId());
    msg.SetString(observer.GetMqDevName());

    SendMsg(msg);
    SPR_LOGD("Unregister observer: [0x%x] [%s]\n", observer.GetModuleId(), observer.GetMqDevName().c_str());
    return 0;
}

int SprMediatorIpcProxy::SendMsg(const SprMsg& msg)
{
    std::string datas;

    msg.Encode(datas);
    int ret = mq_send(mMediatorHandle, (const char*)datas.c_str(), datas.size(), 1);
    if (ret < 0) {
        SPR_LOGE("mq_send failed! (%s)\n", strerror(errno));
    }

    return ret;
}

int SprMediatorIpcProxy::NotifyObserver(const SprMsg& msg)
{
    SendMsg(msg);
    return 0;
}

int SprMediatorIpcProxy::NotifyAllObserver(const SprMsg& msg)
{
    // The default value of mTo in SprMsg is MODULE_NONE.
    // If you want to notify all, do not modify the mTo value in SprMsg when using
    NotifyObserver(msg);
    return 0;
}
