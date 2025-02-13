/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprObserver.h
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
#ifndef __SPR_OBSERVER_H__
#define __SPR_OBSERVER_H__

#include <memory>
#include <string>
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
     * @param proxyType mediator proxy type
     */
    SprObserver(ModuleIDType id, const std::string& name, InternalDefs::EProxyType proxyType);
    virtual ~SprObserver();

    /**
     * @brief Initialize observer
     *
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int32_t Initialize() final;

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
    virtual int32_t NotifyObserver(SprMsg& msg);
    virtual int32_t NotifyObserver(ModuleIDType id, SprMsg& msg);

    /**
     * @brief  Notify msg to all modules
     *
     * @param msg
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int32_t NotifyAllObserver(SprMsg& msg);

    /**
     * @brief Starts a timer with a message ID.
     *
     * @param delayInMSec The delay time in milliseconds before the first trigger.
     *                     A value of 0 indicates that the timer triggers immediately.
     * @param intervalInMSec The interval time in milliseconds between subsequent triggers.
     * @param msgId The message ID associated with the timer.
     * @param repeatTimes The times of the timer repeat trigger.
     *                    0 for an long-term timer,
     *                    others for repeat times.
     * @return 0 on success, or -1 if an error occurred.
     */
    int32_t RegisterTimer(int32_t delayInMSec, int32_t intervalInMSec, uint32_t msgId, uint32_t repeatTimes = 1);

    /**
     * @brief Stops a timer identified by a message ID.
     *
     * @param msgId The message ID of the timer to stop.
     * @return 0 on success, or -1 if an error occurred.
     */
    int32_t UnregisterTimer(uint32_t msgId);

protected:
    /**
     * @brief Initializes the framework module with overrides from derived framework modules
     *
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int32_t InitFramework();

     /**
     * @brief Initializes the business module with overrides from derived business modules
     *
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int32_t Init();

    /**
     * @brief Dump common versions for current module
     * @return 0
     *
     * Used for verifying the consistency of definitions across each component
     */
    int32_t DumpCommonVersion();

protected:
    InternalDefs::EProxyType mProxyType;
    ModuleIDType mModuleID;
    std::string mModuleName;
    SprMediatorProxy* mpMsgMediator;
};

#endif  // __SPR_OBSERVER_H__
