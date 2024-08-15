/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprObserverr.h
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
#ifndef __SPR_NEW_OBSERVER_H__
#define __SPR_NEW_OBSERVER_H__

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
    /**
     * @brief  Construct/Destruct
     *
     * @param id Module id
     * @param name Module name
     * @param mMsgMediatorPtr Message passing mediation
     */
    SprObserver(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr);
    virtual ~SprObserver();
    SprObserver(const SprObserver&) = delete;
    SprObserver& operator=(SprObserver&) = delete;
    SprObserver(SprObserver&&) = delete;
    SprObserver& operator=(SprObserver&&) = delete;

    /**
     * @brief Get the module id
     *
     * @return Module id
     */
    ModuleIDType GetModuleId() const { return mModuleID; }

    /**
     * @brief Get the module name
     *
     * @return Module name
     */
    std::string GetModuleName() const { return mModuleName; }

    /**
     * @brief  Notify msg to other module
     *
     * @param id module id
     * @param msg
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int NotifyObserver(SprMsg& msg);
    virtual int NotifyObserver(ModuleIDType id, SprMsg& msg);

    /**
     * @brief  Notify msg to all modules
     *
     * @param msg
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int NotifyAllObserver(SprMsg& msg);

    /**
     * @brief Blocking wait to listen messages.
     *
     * @return 0 on success, or -1 if an error occurred
     *
     * This function blocks the current thread until a message is received.
     * It utilizes an epoll loop to wait for incoming messages efficiently.
     */
    static int MainLoop();

    /**
     * @brief Exits from MainLoop and performs necessary cleanup.
     *
     * @return 0 on success, or -1 if an error occurred
     *
     * This function should be called to gracefully exit the application's
     * main processing loop. It performs necessary cleanup tasks before
     * terminating the process.
     */
    static int MainExit();

private:
    /**
     * @brief Dump common versions for current module
     * @return 0
     *
     * Used for verifying the consistency of definitions across each component.
     */
    int DumpCommonVersion();

private:
    ModuleIDType mModuleID;
    std::string mModuleName;
    std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr;
};

#endif
