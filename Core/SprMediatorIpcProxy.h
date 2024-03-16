/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorIpcProxy.cpp
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
#include "SprMediatorProxy.h"

class SprMediatorIpcProxy : public SprMediatorProxy
{
public:
    /**
     * @brief Construct a new Spr Mediator Ipc Proxy object
     *
     */
    SprMediatorIpcProxy();

    /**
     * @brief Destroy the Spr Mediator Ipc Proxy object
     *
     */
    virtual ~SprMediatorIpcProxy();

    /**
     * @brief ConnectMediator
     *
     * @return int
     */
    int ConnectMediator() override;

    /**
     * @brief RegisterObserver
     *
     * @param observer
     * @return int
     */
    int RegisterObserver(const SprObserver& observer) override;

    /**
     * @brief UnregisterObserver
     *
     * @param observer
     * @return int
     */
    int UnregisterObserver(const SprObserver& observer) override;

    virtual int SendMsg(const SprMsg& msg) override;
    virtual int NotifyObserver(const SprMsg& msg) override;
    virtual int NotifyAllObserver(const SprMsg& msg) override;

private:
    int mMediatorHandle;
};
