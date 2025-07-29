/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediator.h
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
#ifndef __SPR_MEDIATOR_H__
#define __SPR_MEDIATOR_H__

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mqueue.h>
#include "SprMsg.h"
#include "PMsgQueue.h"
#include "CoreTypeDefs.h"
#include "CommonTypeDefs.h"
#include "SprMQueueDetails.h"

struct SModuleInfo
{
    bool monitored;
    std::shared_ptr<PMsgQueue> pModMQ;
};

class SprMediator
{
public:
    static SprMediator* GetInstance();

    int Init();
    int GetAllMQStatus(std::vector<SMQueueDetails> &mqInfoList);
    std::string GetSignalName(int sig);

private:
    SprMediator();
    ~SprMediator();
    int InitInternalPort();
    int LoadMQStaticInfo(int handle, const std::string& devName);
    int NotifyObserver(InternalDefs::ESprModuleID id, const SprMsg& msg);
    int NotifyAllObserver(const SprMsg& msg);

    /* 消息处理函数 */
    int32_t ProcessMsg(const SprMsg& msg);

    /* 消息响应函数 */
    int MsgRespondRegister(const SprMsg& msg);
    int MsgRespondUnregister(const SprMsg& msg);

private:
    std::shared_ptr<PMsgQueue> mpInternalMQ;
    std::map<int, std::shared_ptr<SprMQueueDetails>> mMQDetailsMap;  // handle, mq
    std::map<InternalDefs::ESprModuleID, SModuleInfo> mModuleMap;
};

#endif // __SPR_MEDIATOR_H__

