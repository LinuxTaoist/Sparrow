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
#include <errno.h>
#include <string.h>
#include "SprCommonType.h"
#include "SprMediatorIpcProxy.h"
#include "SprObserver.h"
#include "SprMsg.h"

using namespace std;

#define SPR_LOGD(fmt, args...) printf("%d IpcProxy D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d IpcProxy E: " fmt, __LINE__, ##args)

const string MEDIATOR_MSG_QUEUE_PORT     = "/SprMdrQ_20231126";          // mediator mqueue

SprMediatorIpcProxy::SprMediatorIpcProxy()
{
    mConnected = false;
    mMediatorHandler = -1;
}

SprMediatorIpcProxy::~SprMediatorIpcProxy()
{
    if (mMediatorHandler != -1)
    {
        mq_close(mMediatorHandler);
        mMediatorHandler = -1;
    }
}

int SprMediatorIpcProxy::mkMq()
{
    mq_attr mqAttr;
    mqAttr.mq_maxmsg = 10;      // cat /proc/sys/fs/mqueue/msg_max
    mqAttr.mq_msgsize = 1025;

    mMediatorHandler = mq_open(MEDIATOR_MSG_QUEUE_PORT.c_str(), O_RDONLY, &mqAttr);
    if(mMediatorHandler < 0) {
        SPR_LOGE("Open %s failed. (%s)\n", MEDIATOR_MSG_QUEUE_PORT.c_str(), strerror(errno));
        return -1;
    }

    return 0;
}

int SprMediatorIpcProxy::ConnectMediator()
{

    return 0;
}

int SprMediatorIpcProxy::RegisterObserver(const SprObserver& observer)
{
    // TODO: 将observer信息发送给远程消息中介注册
    SprMsg msg((uint32_t)EProxyMsgID::PROXY_MSG_REGISTER_REQUEST);
    msg.setU32Value((uint32_t)EProxyType::PROXY_TYPE_MQ);
    msg.setU16Value((uint16_t)observer.getModuleId());
    msg.setString(observer.getMqDevName());

    SPR_LOGD("Register observer: [0x%x] [%s]\n", (uint32_t)EProxyMsgID::PROXY_MSG_REGISTER_REQUEST, observer.getMqDevName().c_str());
    return 0;
}

int SprMediatorIpcProxy::UnregisterObserver(const SprObserver& observer)
{
    // TODO: 将observer信息发送给远程消息中介注册
    SprMsg msg((uint32_t)EProxyMsgID::PROXY_MSG_UNREGISTER_REQUEST);
    msg.setU32Value((uint32_t)EProxyType::PROXY_TYPE_MQ);
    msg.setU16Value((uint16_t)observer.getModuleId());
    msg.setString(observer.getMqDevName());

    SPR_LOGD("Register observer: [0x%x] [%s]\n", (uint32_t)EProxyMsgID::PROXY_MSG_REGISTER_RESPONSE, observer.getMqDevName().c_str());
    return 0;
}

int SprMediatorIpcProxy::NotifyAllObserver()
{
    return 0;
}


