/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediator.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_MEDIATOR_H__
#define __SPR_MEDIATOR_H__

#include "SprCommonType.h"

class SprMediator
{
public:
    SprMediator();
    ~SprMediator();
    int RegisterObserver(ESprModuleID id, std::string name);
    int UnregisterObserver(ESprModuleID id, std::string name);

private:
};

#endif

