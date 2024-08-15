/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorProxy.h
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
#ifndef __SPR_MEDIATOR_PROXY_H__
#define __SPR_MEDIATOR_PROXY_H__

#include "SprMsg.h"

class SprObserver;
class SprMediatorProxy
{
public:
    /**
     * @brief Construct/Destruct
     */
    SprMediatorProxy() {}
    virtual ~SprMediatorProxy() {}
    SprMediatorProxy(const SprMediatorProxy&) = delete;
    SprMediatorProxy& operator=(SprMediatorProxy&) = delete;
    SprMediatorProxy(SprMediatorProxy&&) = delete;
    SprMediatorProxy& operator=(SprMediatorProxy&&) = delete;

    /**
     * @brief Connect to SprMediator
     *
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int ConnectMediator() = 0;

    /**
     * @brief Register observer to SprMediator
     *
     * @param observer
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int RegisterObserver(const SprObserver& observer) = 0;

    /**
     * @brief Unregister observer to SprMediator
     *
     * @param observer
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int UnregisterObserver(const SprObserver& observer) = 0;

    /**
     * @brief send message to other module
     *
     * @param msg
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int NotifyObserver(const SprMsg& msg) = 0;

    /**
     * @brief send message to all module
     *
     * @param msg
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int NotifyAllObserver(const SprMsg& msg) = 0;
};

#endif
