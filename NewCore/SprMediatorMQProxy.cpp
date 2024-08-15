/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorProxy.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
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
#include "SprLog.h"
#include "CoreTypeDefs.h"
#include "SprObserver.h"
#include "SprMediatorMQProxy.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("MQProxy", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("MQProxy", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("MQProxy", fmt, ##args)

SprMediatorMQProxy::SprMediatorMQProxy()
{
    mMdtFd = -1;
    ConnectMediator();
}

SprMediatorMQProxy::~SprMediatorMQProxy()
{
    if (mMdtFd != -1)
    {
        mq_close(mMdtFd);
        mMdtFd = -1;
    }
}

SprMediatorMQProxy* SprMediatorMQProxy::GetInstance()
{
    static SprMediatorMQProxy instance;
    return &instance;
}

int SprMediatorMQProxy::ConnectMediator()
{
    mq_attr mqAttr;
    mqAttr.mq_maxmsg = 10;      // cat /proc/sys/fs/mqueue/msg_max
    mqAttr.mq_msgsize = 1025;

    mMdtFd = mq_open(MEDIATOR_MSG_QUEUE, O_RDWR, 0666, &mqAttr);
    if(mMdtFd < 0) {
        SPR_LOGE("Open %s failed. (%s)\n", MEDIATOR_MSG_QUEUE, strerror(errno));
        return -1;
    }

    return 0;
}

int SprMediatorMQProxy::RegisterObserver(const SprObserver& observer)
{
    SprMsg msg(observer.GetModuleId(), MODULE_PROXY, SIG_ID_PROXY_REGISTER_REQUEST);
    msg.SetBoolValue(true);
    // msg.SetU32Value((uint32_t)PROXY_TYPE_MQ);
    msg.SetU16Value((uint16_t)observer.GetModuleId());
    // msg.SetString(observer.GetMqDevName());

    SendMsg(msg);
    SPR_LOGD("Register observer: [0x%x]\n", observer.GetModuleId());
    return 0;
}

int SprMediatorMQProxy::UnregisterObserver(const SprObserver& observer)
{
    SprMsg msg(observer.GetModuleId(), MODULE_PROXY, SIG_ID_PROXY_UNREGISTER_REQUEST);
    // msg.SetU32Value((uint32_t)PROXY_TYPE_MQ);
    msg.SetU16Value((uint16_t)observer.GetModuleId());
    // msg.SetString(observer.GetMqDevName());

    SendMsg(msg);
    SPR_LOGD("Unregister observer: [0x%x]\n", observer.GetModuleId());
    return 0;
}

int SprMediatorMQProxy::SendMsg(const SprMsg& msg)
{
    std::string datas;

    msg.Encode(datas);
    int ret = mq_send(mMdtFd, datas.c_str(), datas.size(), 1);
    if (ret < 0) {
        SPR_LOGE("mq_send failed! (%s)\n", strerror(errno));
    }

    return ret;
}

int SprMediatorMQProxy::NotifyObserver(const SprMsg& msg)
{
    SendMsg(msg);
    return 0;
}

int SprMediatorMQProxy::NotifyAllObserver(const SprMsg& msg)
{
    // The default value of mTo in SprMsg is MODULE_NONE.
    // If you want to notify all, do not modify the mTo value in SprMsg when using
    NotifyObserver(msg);
    return 0;
}
