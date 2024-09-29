/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorInterface.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/04/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/04/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_MEDIATOR_INTERFACE_H__
#define __SPR_MEDIATOR_INTERFACE_H__

#include <vector>
#include <mqueue.h>
#include "CommonTypeDefs.h"

class SprMediatorInterface
{
public:
    /**
     * @brief  Destructor
     */
    ~SprMediatorInterface();

    /**
     * @brief  Get the instance
     *
     * @return  Single instance
     */
    static SprMediatorInterface* GetInstance();

    /**
     * @brief  Get the all message queue of all modules
     *
     * @param mqAttrVec  Message queue attribute vector
     * @return  0 if success, -1 if failed
     */
    int GetAllMQStatus(std::vector<SMQStatus>& mqAttrVec);

private:
    /**
     * @brief Constructor
     *
     */
    SprMediatorInterface();
};

#endif // __SPR_MEDIATOR_INTERFACE_H__
