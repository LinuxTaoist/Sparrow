/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprCommonType.h
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
#ifndef __SPR_COMMON_TYPE_H__
#define __SPR_COMMON_TYPE_H__

#include <string>
#include <stdint.h>

enum class ESprModuleID
{
    MODULE_MSG_MEDIATOR = 0x01,
    MODULE_CONFIG,
    MODULE_LOGM,
    MODULE_MAX,
};

struct SModuleInfo
{
    ESprModuleID id;
    int handle;
    std::string name;
};

#endif
