/**
 *---------------------------------------------------------------------------------------------------------------------
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
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_OBSERVER_H__
#define __SPR_OBSERVER_H__

#include <set>
#include <vector>
#include <memory>
#include <string>
#include "SprMsg.h"
#include "SprSigId.h"
#include "CoreTypeDefs.h"
#include "SprMediatorProxy.h"

using ModuleIDType = InternalDefs::ESprModuleID;

class SprObserver
{
public:
    SprObserver(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr, bool monitored = true);
    virtual ~SprObserver();
    SprObserver(const SprObserver&) = delete;
    SprObserver& operator=(SprObserver&) = delete;
    SprObserver(SprObserver&&) = delete;
    SprObserver& operator=(SprObserver&&) = delete;

    int GetMqHandle() const { return mMqHandle; }
    ModuleIDType GetModuleId() const { return mModuleID; }
    std::string GetModuleName() const { return mModuleName; }
    std::string GetMqDevName() const { return mMqDevName; }

    bool IsMonitored() const;
    virtual int NotifyObserver(const SprMsg& msg);
    virtual int NotifyAllObserver(const SprMsg& msg);
    virtual int HandleEvent(int fd);
    virtual int ProcessMsg(const SprMsg& msg) = 0;

    int  AbstractProcessMsg(const SprMsg& msg);

    /**
     * @brief  AddPoll
     * @param[in] listenType    listen type in epoll
     * @param[in] fd listen handler in epoll
     * @return 0 on success, or -1 if an error occurred
     *
     * Use this function to add custom listening events to Epoll
     */
    int AddPoll(int fd, uint8_t type);
    int HandlePollEvent(int fd, uint8_t ipcType);

    /**
     * @brief SendMsg
     *
     * @param[in] msg
     * @return 0 on success, or -1 if an error occurred
     *
     * Send message to self message queue
     */
    int SendMsg(const SprMsg& msg);

    /**
     * @brief RecvMsg
     *
     * @param[out] msg
     * @return 0 on success, or -1 if an error occurred
     *
     * Receive message from message queue
     */
    int RecvMsg(SprMsg& msg);

    /**
     * @brief  MainLoop
     * @return 0 on success, or -1 if an error occurred
     */
    static int MainLoop();

    /**
     * @brief  MainExit
     * @return 0 on success, or -1 if an error occurred
     */
    static int MainExit();

private:
    int MakeMQ();
    void InitSigHandler();

    // --------------------------------------------------------------------------------------------
    // - Message handle functions
    // --------------------------------------------------------------------------------------------
    int MsgResponseSystemExitRsp(const SprMsg& msg);
    int MsgResponseRegisterRsp(const SprMsg& msg);
    int MsgResponseUnregisterRsp(const SprMsg& msg);

private:
    bool mConnected;
    bool mListenMQ;
    int  mMqHandle;
    ModuleIDType mModuleID;
    std::string mModuleName;
    std::string mMqDevName;
    std::set<int> mPollFds;
    std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr;
};

#endif
