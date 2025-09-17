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
#ifndef __SPR_OBSERVER_WITH_MQUEUE_H__
#define __SPR_OBSERVER_WITH_MQUEUE_H__

#include "SprMsg.h"
#include "PMsgQueue.h"
#include "SprObserver.h"
#include "CommonTypeDefs.h"
#include "SprMQueueDetails.h"

class SprObserverWithMQueue : public SprObserver, public PMsgQueue
{
public:
    /**
     * @brief  Construct / Destruct
     *
     * @param id module id
     * @param name module name
     * @param proxyType message proxy type
     */
    SprObserverWithMQueue(ModuleIDType id, const std::string& name, InternalDefs::EProxyType proxyType = InternalDefs::MEDIATOR_PROXY_MQUEUE);
    virtual ~SprObserverWithMQueue();

    /**
     * @brief SendMsg
     *
     * @param[in] msg
     * @param[in] msgId message id
     * @return 0 on success, or -1 if an error occurred
     *
     * Send message to self module
     */
    int32_t SendMsg(SprMsg& msg);
    int32_t SendMsg(uint32_t msgId);

    /**
     * @brief  Process message event called by epoll
     *
     * @param fd file descriptor
     * @param eType event type
     * @param arg user data
     *
     * @return void*
     */
    virtual void* EpollEvent(int fd, EpollType eType, void* arg) final override;

protected:
    /**
     * @brief Initialize function for derived class called in Initialize
     *
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int32_t InitFramework() override;

    /**
     * @brief  Process message from message queue received
     *
     * @param msg
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int32_t ProcessMsg(const SprMsg& msg) = 0;

    /**
     * @brief Register self information from mediator module
     *
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t RegisterFromMediator();

    /**
     * @brief Unregister self information from mediator module
     *
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t UnRegisterFromMediator();

    /**
     * @brief Load/Remove message queue information
     *
     * @param handle
     * @param msg
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t LoadMQStaticInfo(int32_t handle, const std::string& devName);
    int32_t LoadMQDynamicInfo(int32_t handle, const SprMsg& msg);

    /**
     * @brief Send event to monitor module
     *
     * @param errcode
     * @param text
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t SendEventToMonitor(int32_t errcode, const std::string& text);

    /**
     * @brief Dispatch messages from message queue
     *
     * @param msg
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t DispatchSprMsg(const SprMsg& msg);

    /**
     * @brief RecvMsg
     *
     * @param[out] msg
     * @return 0 on success, or -1 if an error occurred
     *
     * Receive message from self or other modules
     */
    int32_t RecvMsg(SprMsg& msg);

    // --------------------------------------------------------------------------------------------
    // - Message handle functions
    // --------------------------------------------------------------------------------------------
    int32_t MsgRespondSystemExitRsp(const SprMsg& msg);
    int32_t MsgRespondRegisterRsp(const SprMsg& msg);
    int32_t MsgRespondUnregisterRsp(const SprMsg& msg);
    int32_t MsgRespondPropertyChangedRsp(const SprMsg& msg);

private:
    bool mConnected;
    std::shared_ptr<SprMQueueDetails> mpDetails;
};

#endif // __SPR_OBSERVER_WITH_MQUEUE_H__
