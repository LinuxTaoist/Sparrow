/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprObserverr.h
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
#ifndef __SPR_OBSERVER_H__
#define __SPR_OBSERVER_H__

#include <vector>
#include <memory>
#include <string>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include "SprMsg.h"
#include "SprCommonType.h"
#include "SprMediatorProxy.h"

class SprObserver
{
public:
    using ModuleIDType = InternalEnum::ESprModuleID;

    SprObserver(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr);
    virtual ~SprObserver();
    SprObserver(const SprObserver&) = delete;
    SprObserver& operator=(SprObserver&) = delete;
    SprObserver(SprObserver&&) = delete;
    SprObserver& operator=(SprObserver&&) = delete;

    virtual int GetMqHandle() const final { return mMqHandle; }
    virtual ModuleIDType GetModuleId() const final { return mModuleID; }
    virtual std::string GetModuleName() const final { return mModuleName; }
    virtual std::string GetMqDevName() const final { return mMqDevName; }
    virtual int NotifyObserver(ModuleIDType id, const SprMsg& msg);
    virtual int NotifyAllObserver(const SprMsg& msg);
    virtual int ProcessMsg(const SprMsg& msg) = 0;

    int Start();
    int AbstractProcessMsg(const SprMsg& msg);
    int SendMsg(const SprMsg& msg);
    int RecvMsg(SprMsg& msg);

private:
    bool mConnected;
    int mMqHandle;
    ModuleIDType mModuleID;
    std::string mModuleName;
    std::string mMqDevName;
    std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr;

    virtual int MakeMQ() final;
    int MsgResponseRegisterRsp(const SprMsg& msg);
    int MsgResponseUnregisterRsp(const SprMsg& msg);
};

#endif
