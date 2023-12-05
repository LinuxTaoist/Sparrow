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

enum class ESprModuleID : uint16_t
{
    MODULE_PROXY = 0x01,
    MODULE_CONFIG,
    MODULE_LOGM,
    MODULE_DEBUG,
    MODULE_MAX,
};

struct SModuleInfo
{
    ESprModuleID id;
    int handle;
    std::string name;
};

enum class EProxyType
{
    PROXY_TYPE_MQ = 1,
    PROXY_TYPE_UNIX_SOCKET,
    PROXY_TYPE_INET_SOCKET,
    PROXY_TYPE_BUTT
};

enum class EProxyMsgID
{
    PROXY_MSG_BEGIN = static_cast<int>(ESprModuleID::MODULE_PROXY) << 16 | 1,
    PROXY_MSG_REGISTER_REQUEST,
    PROXY_MSG_REGISTER_RESPONSE,
    PROXY_MSG_UNREGISTER_REQUEST,
    PROXY_MSG_UNREGISTER_RESPONSE,
    PROXY_MSG_BUTT
};

enum class EDebugMsgID
{
    DEBUG_MSG_BEGIN = static_cast<int>(ESprModuleID::MODULE_DEBUG) << 16 | 1,
    DEBUG_MSG_SERIAL,
    DEBUG_MSG_BUTT
};

#endif
