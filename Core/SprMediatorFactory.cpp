/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorFactory.cpp
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
#include "SprMediatorFactory.h"
#include "SprMediatorMQProxy.h"

using namespace InternalDefs;

SprMediatorFactory::SprMediatorFactory()
{
}

SprMediatorFactory::~SprMediatorFactory()
{
}

SprMediatorFactory* SprMediatorFactory::GetInstance()
{
    static SprMediatorFactory instance;
    return &instance;
}

SprMediatorProxy* SprMediatorFactory::GetMediatorProxy(InternalDefs::EProxyType type)
{
    SprMediatorProxy* pProxy = nullptr;
    switch (type) {
        case MEDIATOR_PROXY_MQUEUE: {
            pProxy = SprMediatorMQProxy::GetInstance();
            break;
        }
        case MEDIATOR_PROXY_SOCKET: {
            break;
        }
        default: {
            break;
        }
    }

    return pProxy;
}



