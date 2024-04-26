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
#include <thread>
#include <mqueue.h>
#include "SprMsg.h"
#include "CoreTypeDefs.h"
#include "CommonTypeDefs.h"

struct SModuleInfo
{
    bool monitored;
    int handle;
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
    explicit SprMediator(int size);
    int EnvReady(const std::string& srvName);
    int MakeMQ(const std::string& name);
    int StartBinderThread();
    int PrepareInternalPort();
    int DestroyInternalPort();
    int GetAllMQStatus(std::vector<SMQStatus> &mqInfoList);
    int LoadMQStaticInfo(int handle, const std::string& devName);
    int LoadMQDynamicInfo(int handle, const SprMsg& msg);
    // int SendMsg(const SprMsg& msg);
    int NotifyObserver(InternalDefs::ESprModuleID id, const SprMsg& msg);
    int NotifyAllObserver(const SprMsg& msg);

    /* 消息处理函数 */
    int ProcessMsg(const SprMsg& msg);

    /* 消息响应函数 */
    int MsgResponseRegister(const SprMsg& msg);
    int MsgResponseUnregister(const SprMsg& msg);

    /* Binder 线程处理函数 */
    static void BinderLoop(SprMediator* self);

private:
    int mHandler;
    int mEpollHandler;
    bool mBinderRunning;
    std::string mMqDevName;
    std::thread mBinderThread;
    std::map<int, SMQStatus> mMQStatusMap;  // handle, mq status
    std::map<InternalDefs::ESprModuleID, SModuleInfo> mModuleMap;
};

#endif

