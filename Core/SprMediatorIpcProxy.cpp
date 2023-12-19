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
using namespace InternalEnum;

#define SPR_LOGD(fmt, args...) printf("%d IpcProxy D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d IpcProxy E: " fmt, __LINE__, ##args)

SprMediatorIpcProxy::SprMediatorIpcProxy()
{
    mMediatorHandler = -1;
    ConnectMediator();
}

SprMediatorIpcProxy::~SprMediatorIpcProxy()
{
    if (mMediatorHandler != -1)
    {
        mq_close(mMediatorHandler);
        mMediatorHandler = -1;
    }
}

int SprMediatorIpcProxy::ConnectMediator()
{
    mq_attr mqAttr;
    mqAttr.mq_maxmsg = 10;      // cat /proc/sys/fs/mqueue/msg_max
    mqAttr.mq_msgsize = 1025;

    mMediatorHandler = mq_open(MEDIATOR_MSG_QUEUE, O_RDWR, &mqAttr);
    if(mMediatorHandler < 0) {
        SPR_LOGE("Open %s failed. (%s)\n", MEDIATOR_MSG_QUEUE, strerror(errno));
        return -1;
    }

    return 0;
}

int SprMediatorIpcProxy::RegisterObserver(const SprObserver& observer)
{
    SprMsg msg(PROXY_MSG_REGISTER_REQUEST);
    msg.SetU32Value((uint32_t)PROXY_TYPE_MQ);
    msg.SetU16Value((uint16_t)observer.GetModuleId());
    msg.SetString(observer.GetMqDevName());

    SendMsg(msg);
    SPR_LOGD("Register observer: [0x%x] [%s]\n", PROXY_MSG_REGISTER_REQUEST, observer.GetMqDevName().c_str());
    return 0;
}

int SprMediatorIpcProxy::UnregisterObserver(const SprObserver& observer)
{
    SprMsg msg(PROXY_MSG_UNREGISTER_REQUEST);
    msg.SetU32Value((uint32_t)PROXY_TYPE_MQ);
    msg.SetU16Value((uint16_t)observer.GetModuleId());
    msg.SetString(observer.GetMqDevName());

    SendMsg(msg);
    SPR_LOGD("Unregister observer: [0x%x] [%s]\n", PROXY_MSG_REGISTER_RESPONSE, observer.GetMqDevName().c_str());
    return 0;
}

int SprMediatorIpcProxy::SendMsg(const SprMsg& msg)
{
    std::string datas;

    msg.Encode(datas);
    int ret = mq_send(mMediatorHandler, (const char*)datas.c_str(), datas.size(), 1);
    if (ret < 0) {
        SPR_LOGE("mq_send failed! (%s)\n", strerror(errno));
    }

    return ret;
}

int SprMediatorIpcProxy::NotifyAllObserver(const SprMsg& msg)
{
    SendMsg(msg);
    return 0;
}
