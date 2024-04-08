/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediator.h
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
#ifndef __SPR_MEDIATOR_H__
#define __SPR_MEDIATOR_H__

#include <map>
#include <string>
#include <vector>
#include "SprMsg.h"
#include "SprCommonType.h"

struct SModuleInfo
{
    bool monitored;
    int handler;
    std::string name;
};

class SprMediator
{
public:
    ~SprMediator();
    static SprMediator* GetInstance();
    int Init();
    int EpollLoop();

private:
    SprMediator(int size);
    int EnvReady(const std::string& srvName);
    int MakeMQ(std::string name);
    int MakeUnixDgramSocket(std::string ip, uint16_t port);
    int PrepareInternalPort();
    int DestroyInternalPort();
    int SendMsg(const SprMsg& msg);
    int NotifyObserver(InternalEnum::ESprModuleID id, const SprMsg& msg);
    int NotifyAllObserver(const SprMsg& msg);

    /* 消息处理函数 */
    int ProcessMsg(const SprMsg& msg);

    /* 消息响应函数 */
    int MsgResponseRegister(const SprMsg& msg);
    int MsgResponseUnregister(const SprMsg& msg);

private:
    int mHandler;
    int mEpollHandler;
    std::map<InternalEnum::ESprModuleID, SModuleInfo> mModuleMap;
};

#endif

