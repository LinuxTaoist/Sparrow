/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprObserverWithSocket.cpp
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
#include <iostream>
#include <iomanip>
#include <sstream>
#include "SprLog.h"
#include "SprEpollSchedule.h"
#include "SprObserverWithSocket.h"

using namespace InternalDefs;

#define SPR_LOGD(fmt, args...)  LOGD("SprObsSocket", "[%s] " fmt, ##args)
#define SPR_LOGW(fmt, args...)  LOGW("SprObsSocket", "[%s] " fmt, ##args)
#define SPR_LOGE(fmt, args...)  LOGE("SprObsSocket", "[%s] " fmt, ##args)

SprObserverWithSocket::SprObserverWithSocket(int sock, const std::function<void(int, void*)>& cb, void* arg)
    : SprObserver(MODULE_NONE, "", IPC_TYPE_BUTT), PSocket(sock, cb, arg)
{
}

SprObserverWithSocket::SprObserverWithSocket(int domain, int type, int protocol, const std::function<void(int, void*)>& cb, void* arg)
    : SprObserver(MODULE_NONE, "", IPC_TYPE_BUTT), PSocket(domain, type, protocol, cb, arg)
{
}

SprObserverWithSocket::SprObserverWithSocket(ModuleIDType id, const std::string& name, InternalDefs::EProxyType proxyType,
    int domain, int type, int protocol, std::function<void(int, void*)> cb, void* arg)
    : SprObserver(id, name, proxyType), PSocket(domain, type, protocol, cb, arg)
{
}

SprObserverWithSocket::~SprObserverWithSocket()
{
    // SPR_LOGD("DelPoll socket observer! fd = %d\n", GetEpollFd());
    SprEpollSchedule::GetInstance()->DelPoll(this);
}

int32_t SprObserverWithSocket::InitFramework()
{
    // SPR_LOGD("AddPoll socket observer! fd = %d\n", GetEpollFd());
    SprEpollSchedule::GetInstance()->AddPoll(this);
    return 0;
}
