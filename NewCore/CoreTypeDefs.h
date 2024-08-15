/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CoreTypeDefs.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/11/25
 *
 * This header serves as a central repository for standardized definitions to enhance code clarity,
 * facilitate maintenance, and minimize duplication across various internal modules within the project.
 * It encapsulates fundamental data types ubiquitously understood and employed across different system components.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __CORE_TYPE_DEFS_H__
#define __CORE_TYPE_DEFS_H__

#include <string>
#include <stdint.h>

// --------------------------------------------------------------------------------------------------------------------
// - Common macros for CoreTypeDefs.h version
// --------------------------------------------------------------------------------------------------------------------
#define CORE_TYPE_DEFS_VERSION   "CORE_TYPE_DEFS_VERSION_R1001"

// --------------------------------------------------------------------------------------------------------------------
// - Common macros for core commponents
// --------------------------------------------------------------------------------------------------------------------
#define     MEDIATOR_MSG_QUEUE          "/SprMdrQ_20231126"          // mqueue
#define     MEDIATOR_UNIX_DGRAM_PATH    "/tmp/SprMdrU_20231126"      // unix socket
#define     MEDIATOR_INET_PORT          1126
#define     MSG_BUF_MAX_LENGTH          1024
#define     MODULE_ID_OFFSET            24

namespace InternalDefs {

enum EProxyType
{
    MEDIATOR_PROXY_MQUEUE = 0,
    MEDIATOR_PROXY_SOCKET,
    MEDIATOR_PROXY_TIMERFD,
    IPC_TYPE_BUTT
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
    MODULE_ONENET_MANAGER,
    MODULE_ONENET_DRIVER,
    MODULE_DEBUG,
    MODULE_DEBUG_WATCH,
    MODULE_MAX,
};

enum EProxyBinderCmd
{
    PROXY_CMD_BEGIN = MODULE_PROXY << MODULE_ID_OFFSET | 1,
    PROXY_CMD_GET_ALL_MQ_ATTRS,
    PROXY_CMD_BUTT
};

enum EPropertyBinderCmd
{
    PROPERTY_CMD_BEGIN = MODULE_PROPERTYM << MODULE_ID_OFFSET | 1,
    PROPERTY_CMD_SET_PROPERTY,
    PROPERTY_CMD_GET_PROPERTY,
    PROPERTY_CMD_GET_PROPERTIES,
    PROPERTY_CMD_BUTT,
};

enum EPowerMBinderCmd
{
    POWERM_CMD_BEGIN = MODULE_POWERM << MODULE_ID_OFFSET | 1,
    POWERM_CMD_POWER_ON,
    POWERM_CMD_POWER_OFF,
    POWERM_CMD_BUTT
};

enum EDebugMBinderCmd
{
    DEBUG_CMD_BEGIN = MODULE_DEBUG << MODULE_ID_OFFSET | 1,
    DEBUG_CMD_ENABLE_REMOTE_PORT,
    DEBUG_CMD_DISABLE_REMOTE_PORT,
    DEBUG_CMD_TEST_ADD_1S_TIMER,
    DEBUG_CMD_TEST_DEL_1S_TIMER,
    DEBUG_CMD_TEST_ADD_CUSTOM_TIMER,
    DEBUG_CMD_TEST_DEL_CUSTOM_TIMER,
    DEBUG_CMD_ENABLE_REMOTE_SHELL,
    DEBUG_CMD_DISABLE_REMOTE_SHELL,
    DEBUG_MSG_BUTT
};

enum EModuleBootPriority
{
    BOOT_PRIORITY_HIGHEST       = 0,    // used for environment initialization
    BOOT_PRIORITY_HIGH          = 1,    // used for system core services
    BOOT_PRIORITY_MEDIUM_HIGH   = 2,    // used for critical services, initial project configuration, etc
    BOOT_PRIORITY_MEDIUM        = 3,    // used for main business services
    BOOT_PRIORITY_MEDIUM_LOW    = 4,    // used for ancillary services, not need to start immediately.
    BOOT_PRIORITY_LOWEST        = 5     // used for optional services
};

typedef struct
{
    uint32_t moduleId;
    uint32_t msgId;
    uint32_t repeatTimes;
    int32_t  delayInMilliSec;
    int32_t  intervalInMilliSec;
} STimerInfo;

template <class Lev1State, class Lev2State, class SignalType, class ClassName, class MsgType>
struct StateTransition
{
    Lev1State   lev1State;
    Lev2State   lev2State;
    SignalType	sigId;
    void (ClassName::*callback)(const MsgType& msg);
};

} // namespace InternalDefs

#endif // __CORE_TYPE_DEFS_H__
