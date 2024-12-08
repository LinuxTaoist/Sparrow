/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorBak.h
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
#ifndef __SPR_MEDIATOR_BAK_H__
#define __SPR_MEDIATOR_BAK_H__

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

class SprMediatorBak
{
public:
    ~SprMediatorBak();
    static SprMediatorBak* GetInstance();
    int Init();
    int EpollLoop();
    static int StopWork();

    int GetAllMQStatus(std::vector<SMQStatus> &mqInfoList);
    std::string GetSignalName(int sig);

private:
    explicit SprMediatorBak(int size);
    int EnvReady(const std::string& srvName);
    int MakeMQ(const std::string& name);
    int InitInternalPort();
    int DestroyInternalPort();
    int LoadMQStaticInfo(int handle, const std::string& devName);
    int LoadMQDynamicInfo(int handle, const SprMsg& msg);
    // int SendMsg(const SprMsg& msg);
    int NotifyObserver(InternalDefs::ESprModuleID id, const SprMsg& msg);
    int NotifyAllObserver(const SprMsg& msg);

    /* 消息处理函数 */
    int ProcessMsg(const SprMsg& msg);

    /* 消息响应函数 */
    int MsgRespondRegister(const SprMsg& msg);
    int MsgRespondUnregister(const SprMsg& msg);

private:
    static bool mEpollRunning;
    int mHandler;
    int mEpollHandler;
    std::string mMqDevName;
    std::map<int, SMQStatus> mMQStatusMap;  // handle, mq status
    std::map<InternalDefs::ESprModuleID, SModuleInfo> mModuleMap;
};

#endif // __SPR_MEDIATOR_H__

