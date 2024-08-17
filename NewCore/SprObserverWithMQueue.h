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
     * @brief Register self infomation from mediator module
     *
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t RegisterFromMediator();

    /**
     * @brief Unregister self infomation from mediator module
     *
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t UnRegisterFromMediator();

    /**
     * @brief SendMsg
     *
     * @param[in] msg
     * @return 0 on success, or -1 if an error occurred
     *
     * Send message to self module
     */
    int32_t SendMsg(SprMsg& msg);

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
    int MsgRespondSystemExitRsp(const SprMsg& msg);
    int MsgRespondRegisterRsp(const SprMsg& msg);
    int MsgRespondUnregisterRsp(const SprMsg& msg);

private:
    bool mConnected;
};

#endif // __SPR_OBSERVER_WITH_MQUEUE_H__
