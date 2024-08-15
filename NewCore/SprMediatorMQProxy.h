/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorMQProxy.cpp
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
#ifndef __SPR_MEDIATOR_MQ_PROXY_H__
#define __SPR_MEDIATOR_MQ_PROXY_H__

#include "SprMediatorProxy.h"

class SprMediatorMQProxy : public SprMediatorProxy
{
public:
    /**
     * @brief Destruct
     */
    virtual ~SprMediatorMQProxy();

    /**
     * @brief Get the single instance object
     *
     * @return single instance object
     */
    static SprMediatorMQProxy* GetInstance();

    /**
     * @brief Connect to SprMediator
     *
     * @return 0 on success, or -1 if an error occurred
     */
    int ConnectMediator() override;

    /**
     * @brief Register observer to SprMediator
     *
     * @param observer
     * @return 0 on success, or -1 if an error occurred
     */
    int RegisterObserver(const SprObserver& observer) override;

    /**
     * @brief Unregister observer to SprMediator
     *
     * @param observer
     * @return 0 on success, or -1 if an error occurred
     */
    int UnregisterObserver(const SprObserver& observer) override;

    /**
     * @brief send message to other module
     *
     * @param msg
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int NotifyObserver(const SprMsg& msg) override;

    /**
     * @brief send message to all module
     *
     * @param msg
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int NotifyAllObserver(const SprMsg& msg) override;

protected:
    /**
     * @brief Construct
     */
    SprMediatorMQProxy();

    /**
     * @brief Send message to other module
     *
     * @param msg
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int SendMsg(const SprMsg& msg);

private:
    int mMdtFd;
};

#endif // __SPR_MEDIATOR_MQ_PROXY_H__
