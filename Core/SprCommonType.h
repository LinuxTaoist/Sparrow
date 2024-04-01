/**
 *---------------------------------------------------------------------------------------------------------------------
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
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_COMMON_TYPE_H__
#define __SPR_COMMON_TYPE_H__

#include <string>
#include <stdint.h>

#define     MEDIATOR_MSG_QUEUE          "/SprMdrQ_20231126"          // mqueue
#define     MEDIATOR_UNIX_DGRAM_PATH    "/tmp/SprMdrU_20231126"      // unix socket
#define     MEDIATOR_INET_PORT          1126
#define     MSG_BUF_MAX_LENGTH          1024
#define     MODULE_ID_OFFSET            24

namespace InternalEnum {

enum EProxyType
{
    PROXY_TYPE_MQ = 0,
    PROXY_TYPE_UNIX_SOCKET,
    PROXY_TYPE_INET_SOCKET,
    PROXY_TYPE_BUTT
};

enum ESprModuleID
{
    MODULE_NONE     = 0x00,
    MODULE_PROXY    = 0x01,
    MODULE_PROPERTYM,
    MODULE_CONFIG,
    MODULE_LOGM,
    MODULE_TIMERM,
    MODULE_SYSTEM_TIMER,
    MODULE_POWERM,
    MODULE_DEBUG,
    MODULE_MAX,
};

enum EProxyMsgID
{
    PROXY_MSG_BEGIN = MODULE_PROXY << MODULE_ID_OFFSET | 1,
    PROXY_MSG_BUTT
};

enum EPropertyBinderCmd
{
    PROPERTY_CMD_BEGIN = MODULE_PROPERTYM << MODULE_ID_OFFSET | 1,
    PROPERTY_CMD_SET_PROPERTY,
    PROPERTY_CMD_GET_PROPERTY,
    PROPERTY_CMD_GET_PROPERTIES,
    PROPERTY_CMD_BUTT,
};

enum EDebugMsgID
{
    DEBUG_MSG_BEGIN = MODULE_DEBUG << MODULE_ID_OFFSET | 1,
    DEBUG_MSG_BUTT
};

enum EIPCType : uint8_t
{
    IPC_TYPE_MQ = 0,
    IPC_TYPE_SOCKET,
    IPC_TYPE_TIMERFD,
    IPC_TYPE_BUTT
};

typedef struct
{
    uint32_t ModuleId;
    uint32_t MsgId;
    uint32_t RepeatTimes;
    int32_t  DelayInMilliSec;
    int32_t  IntervalInMilliSec;
} STimerInfo;

} // namespace InternalEnum

#endif
