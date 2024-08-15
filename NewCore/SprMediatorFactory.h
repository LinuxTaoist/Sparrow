/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorFactory.h
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
#ifndef __SPR_MEDIATOR_FACTORY_H__
#define __SPR_MEDIATOR_FACTORY_H__

#include "CoreTypeDefs.h"
#include "SprMediatorProxy.h"

class SprMediatorFactory
{
public:
    /**
     * @brief Destruct
     */
    ~SprMediatorFactory();

    /**
     * @brief Get the instance object
     *
     * @return Single instance of SprMediatorFactory
     */
    static SprMediatorFactory* GetInstance();

    /**
     * @brief Get a mediator proxy
     */
    SprMediatorProxy* GetMediatorProxy(InternalDefs::EProxyType type);

protected:
    /**
     * @brief Construct
     */
    SprMediatorFactory();
};

#endif // __SPR_MEDIATOR_FACTORY_H__
