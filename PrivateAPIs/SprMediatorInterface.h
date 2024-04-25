/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorInterface.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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

class SprMediatorInterface
{
public:
    ~SprMediatorInterface();
    static SprMediatorInterface* GetInstance();

    int GetAllMQAttrs(std::vector<mq_attr>& mqAttrVec);

private:
    bool mEnable;
    SprMediatorInterface();
};

#endif // __SPR_MEDIATOR_INTERFACE_H__
