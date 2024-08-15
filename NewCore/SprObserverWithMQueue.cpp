/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprObserverWithMQueue.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/15
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/08/15 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "SprLog.h"
#include "PMsgQueue.h"
#include "SprMediatorMQProxy.h"
#include "SprObserverWithMQueue.h"

using namespace InternalDefs;

#define SPR_LOGD(fmt, args...)  LOGD("SprObsMQ", "[%s] " fmt, mModuleName.c_str(), ##args)
#define SPR_LOGE(fmt, args...)  LOGE("SprObsMQ", "[%s] " fmt, mModuleName.c_str(), ##args)

SprObserverWithMQueue::SprObserverWithMQueue(ModuleIDType id, const std::string& name, EProxyType proxyType)
    : SprObserver(id, name, proxyType), PMsgQueue(name)
{
}

SprObserverWithMQueue::~SprObserverWithMQueue()
{
}

int32_t SprObserverWithMQueue::SendMsg(SprMsg& msg)
{
    std::string datas;
    msg.SetFrom(mModuleID);
    msg.SetTo(mModuleID);
    msg.Encode(datas);

    int32_t ret = Send(datas);
    if (ret < 0) {
        SPR_LOGE("Send failed!\n");
    }

    return ret;
}

int32_t SprObserverWithMQueue::RecvMsg(SprMsg& msg)
{
    uint32_t prio = 0;
    std::string data;

    int32_t ret = Recv(data, prio);
    if (ret < 0) {
        SPR_LOGE("Recv failed!");
        return -1;
    }

    return msg.Decode(data);
}

void* SprObserverWithMQueue::EpollEvent(int fd, EpollType eType, void* arg)
{
    std::string msg;
    uint32_t prio = 0;
    if (Recv(msg, prio) < 0) {
        return nullptr;
    }

    return nullptr;
}

