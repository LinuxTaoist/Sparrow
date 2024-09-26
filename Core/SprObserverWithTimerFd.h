/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprObserverWithTimerfd.h
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
#ifndef __SPR_OBSERVER_TIMER_FD_H__
#define __SPR_OBSERVER_TIMER_FD_H__

#include "PTimer.h"
#include "SprObserver.h"

class SprObserverWithTimerfd : public SprObserver, public PTimer
{
public:
    /**
     * @brief  Construct/Destruct
     *
     * @param id module id
     * @param name module name
     * @param proxyType message proxy type
     */
    SprObserverWithTimerfd(ModuleIDType id, const std::string& name, InternalDefs::EProxyType proxyType = InternalDefs::MEDIATOR_PROXY_MQUEUE);
    virtual ~SprObserverWithTimerfd();

    /**
     * @brief Initialize function for derived class called in Initialize
     *
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int32_t InitFramework() override;

    /**
     * @brief  Process timer event
     *
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int32_t ProcessTimerEvent() = 0;

    /**
     * @brief  Process message event called by epoll
     *
     * @param fd file descriptor
     * @param eType event type
     * @param arg user data
     *
     * @return void*
     */
    virtual void* EpollEvent(int fd, EpollType eType, void* arg) override;
};

#endif // __SPR_OBSERVER_TIMER_FD_H__
