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

#include <map>
#include <string>
#include <stdint.h>

// --------------------------------------------------------------------------------------------------------------------
// - Common macros for CoreTypeDefs.h version
// --------------------------------------------------------------------------------------------------------------------
#define CORE_TYPE_DEFS_VERSION   "CORE_TYPE_DEFS_VERSION_R1006"

// --------------------------------------------------------------------------------------------------------------------
// - Common defines for Plugin
// --------------------------------------------------------------------------------------------------------------------
#define DEFAULT_PLUGIN_LIBRARY_FILE_PREFIX      "libplugin"
#define DEFAULT_PLUGIN_LIBRARY_PATH             "/mnt/wsl/hdd1/gitee/Sparrow/Release/Lib"
#define DEFAULT_PLUGIN_LIBRARY_ENTRY_FUNC       "PluginEntry"
#define DEFAULT_PLUGIN_LIBRARY_EXIT_FUNC        "PluginExit"

class SprObserver;  // forward declaration
class SprContext;   // forward declaration
typedef void(*PluginEntryFunc) (std::map<int32_t, SprObserver*>& modules, SprContext& ctx);
typedef void(*PluginExitFunc) (std::map<int32_t, SprObserver*>& modules, SprContext& ctx);

// --------------------------------------------------------------------------------------------------------------------
// - Common macros for core commponents
// --------------------------------------------------------------------------------------------------------------------
#define     DEBUG_NODE_ROOT_PATH        "/tmp"
#define     MEDIATOR_MSG_QUEUE          "/SprMdrQ_20231126"          // mqueue
#define     MEDIATOR_UNIX_DGRAM_PATH    "/tmp/SprMdrU_20231126"      // unix socket
#define     MAIN_EXIT_SIGNUM            SIGUSR1
#define     MEDIATOR_INET_PORT          1126
#define     MSG_BUF_MAX_LENGTH          1024
#define     MODULE_ID_OFFSET              24

namespace InternalDefs {

namespace {

#ifdef ENUM_OR_STRING
#undef ENUM_OR_STRING
#endif
#define ENUM_OR_STRING(x) x

}

// --------------------------------------------------------------------------------------------------------------------
// - EProxyType
// --------------------------------------------------------------------------------------------------------------------
#define PROXY_TYPE_MACROS \
    ENUM_OR_STRING(MEDIATOR_PROXY_MQUEUE),  \
    ENUM_OR_STRING(MEDIATOR_PROXY_SOCKET),  \
    ENUM_OR_STRING(MEDIATOR_PROXY_TIMERFD), \
    ENUM_OR_STRING(IPC_TYPE_BUTT)

enum EProxyType
{
    PROXY_TYPE_MACROS
};

//---------------------------------------------------------------------------------------------------------------------
// - ESprModuleID
// --------------------------------------------------------------------------------------------------------------------
#define SPR_MODULE_ID_MACROS \
    ENUM_OR_STRING(MODULE_NONE),                \
    ENUM_OR_STRING(MODULE_PROXY),               \
    ENUM_OR_STRING(MODULE_PROPERTYM),           \
    ENUM_OR_STRING(MODULE_CONFIG),              \
    ENUM_OR_STRING(MODULE_LOGM),                \
    ENUM_OR_STRING(MODULE_TIMEM),               \
    ENUM_OR_STRING(MODULE_TIMERM),              \
    ENUM_OR_STRING(MODULE_SYSTEM_TIMER),        \
    ENUM_OR_STRING(MODULE_POWERM),              \
    ENUM_OR_STRING(MODULE_STATUS_MONITOR),      \
    ENUM_OR_STRING(MODULE_ONENET_MANAGER),      \
    ENUM_OR_STRING(MODULE_ONENET_DEV01),        \
    ENUM_OR_STRING(MODULE_ONENET_DEV02),        \
    ENUM_OR_STRING(MODULE_ONENET_DEV03),        \
    ENUM_OR_STRING(MODULE_ONENET_DEV04),        \
    ENUM_OR_STRING(MODULE_ONENET_DEV05),        \
    ENUM_OR_STRING(MODULE_ONENET_DRIVER),       \
    ENUM_OR_STRING(MODULE_ONENET_SOCKET),       \
    ENUM_OR_STRING(MODULE_ONENET_UNIX_PIPE0),   \
    ENUM_OR_STRING(MODULE_ONENET_UNIX_PIPE1),   \
    ENUM_OR_STRING(MODULE_DEBUG),               \
    ENUM_OR_STRING(MODULE_DEBUG_WATCH),         \
    ENUM_OR_STRING(MODULE_GTEST_INTERNAL),      \
    ENUM_OR_STRING(MODULE_GENERAL),             \
    ENUM_OR_STRING(MODULE_MAX)

enum ESprModuleID
{
    SPR_MODULE_ID_MACROS
};

//---------------------------------------------------------------------------------------------------------------------
// - EProxyBinderCmd
// --------------------------------------------------------------------------------------------------------------------
#define PROXY_BINDER_CMD_MACROS \
    ENUM_OR_STRING(PROXY_CMD_GET_ALL_MQ_ATTRS), \
    ENUM_OR_STRING(PROXY_CMD_GET_SIGNAL_NAME),  \
    ENUM_OR_STRING(PROXY_CMD_BUTT)

enum EProxyBinderCmd
{
    PROXY_CMD_BEGIN = MODULE_PROXY << MODULE_ID_OFFSET | 1,
    PROXY_BINDER_CMD_MACROS
};

//---------------------------------------------------------------------------------------------------------------------
// - EPropertyBinderCmd
// --------------------------------------------------------------------------------------------------------------------
#define PROPERTY_BINDER_CMD_MACROS \
    ENUM_OR_STRING(PROPERTY_CMD_SET_PROPERTY),      \
    ENUM_OR_STRING(PROPERTY_CMD_GET_PROPERTY),      \
    ENUM_OR_STRING(PROPERTY_CMD_GET_PROPERTIES), \
    ENUM_OR_STRING(PROPERTY_CMD_DUMP_PROPERTIES),   \
    ENUM_OR_STRING(PROPERTY_CMD_BUTT)

enum EPropertyBinderCmd
{
    PROPERTY_CMD_BEGIN = MODULE_PROPERTYM << MODULE_ID_OFFSET | 1,
    PROPERTY_BINDER_CMD_MACROS
};

//---------------------------------------------------------------------------------------------------------------------
// - EPowerBinderCmd
// --------------------------------------------------------------------------------------------------------------------
#define POWER_BINDER_CMD_MACROS \
    ENUM_OR_STRING(POWERM_CMD_POWER_ON),    \
    ENUM_OR_STRING(POWERM_CMD_POWER_OFF),   \
    ENUM_OR_STRING(POWERM_CMD_BUTT)

enum EPowerMBinderCmd
{
    POWERM_CMD_BEGIN = MODULE_POWERM << MODULE_ID_OFFSET | 1,
    POWER_BINDER_CMD_MACROS
};

//---------------------------------------------------------------------------------------------------------------------
// - EDebugBinderCmd
// --------------------------------------------------------------------------------------------------------------------
#define DEBUG_BINDER_CMD_MACROS \
    ENUM_OR_STRING(DEBUG_CMD_ENABLE_REMOTE_PORT),       \
    ENUM_OR_STRING(DEBUG_CMD_DISABLE_REMOTE_PORT),      \
    ENUM_OR_STRING(DEBUG_CMD_TEST_ADD_1S_TIMER),        \
    ENUM_OR_STRING(DEBUG_CMD_TEST_DEL_1S_TIMER),        \
    ENUM_OR_STRING(DEBUG_CMD_TEST_ADD_CUSTOM_TIMER),    \
    ENUM_OR_STRING(DEBUG_CMD_TEST_DEL_CUSTOM_TIMER),    \
    ENUM_OR_STRING(DEBUG_CMD_ENABLE_REMOTE_SHELL),      \
    ENUM_OR_STRING(DEBUG_CMD_DISABLE_REMOTE_SHELL),     \
    ENUM_OR_STRING(DEBUG_MSG_BUTT)

enum EDebugMBinderCmd
{
    DEBUG_CMD_BEGIN = MODULE_DEBUG << MODULE_ID_OFFSET | 1,
    DEBUG_BINDER_CMD_MACROS
};

//---------------------------------------------------------------------------------------------------------------------
// - ELogLevel
// --------------------------------------------------------------------------------------------------------------------
#define LOG_LEVEL_MACROS \
    ENUM_OR_STRING(LOG_LEVEL_MIN),   \
    ENUM_OR_STRING(LOG_LEVEL_ERROR), \
    ENUM_OR_STRING(LOG_LEVEL_WARN),  \
    ENUM_OR_STRING(LOG_LEVEL_INFO),  \
    ENUM_OR_STRING(LOG_LEVEL_DEBUG), \
    ENUM_OR_STRING(LOG_LEVEL_BUTT)

enum ELogLevel
{
    LOG_LEVEL_MACROS
};

//---------------------------------------------------------------------------------------------------------------------
// - EOneNetBinderCmd
// --------------------------------------------------------------------------------------------------------------------
#define ONENET_BINDER_CMD_MACROS \
    ENUM_OR_STRING(ONENET_CMD_ACTIVE_DEVICE),   \
    ENUM_OR_STRING(ONENET_CMD_DEACTIVE_DEVICE), \
    ENUM_OR_STRING(ONENET_CMD_BUTT)

enum EOneNetBinderCmd
{
    ONENET_CMD_BEGIN = MODULE_ONENET_MANAGER << MODULE_ID_OFFSET | 1,
    ONENET_BINDER_CMD_MACROS
};

//---------------------------------------------------------------------------------------------------------------------
// - EGeneralBinderCmd
// --------------------------------------------------------------------------------------------------------------------
#define GENERAL_BINDER_CMD_MACROS \
    ENUM_OR_STRING(GENERAL_CMD_EXE_EXIT),           \
    ENUM_OR_STRING(GENERAL_REGISTER_CALLBACK),      \
    ENUM_OR_STRING(GENERAL_UNREGISTER_CALLBACK),    \
    ENUM_OR_STRING(GENERAL_CMD_BUTT)

enum EGeneralBinderCmd
{
    GENERAL_CMD_BEGIN = MODULE_GENERAL << MODULE_ID_OFFSET | 1,
    GENERAL_BINDER_CMD_MACROS
};

//---------------------------------------------------------------------------------------------------------------------
// - EModuleBootPriority
// --------------------------------------------------------------------------------------------------------------------
// BOOT_PRIORITY_HIGHEST        : used for environment initialization
// BOOT_PRIORITY_HIGH           : used for system core services
// BOOT_PRIORITY_MEDIUM         : used for main business services
// BOOT_PRIORITY_LOW            : used for ancillary services, not need to start immediately.
// BOOT_PRIORITY_LOWEST         : used for optional services
#define MODULE_BOOT_PRIORITY_MACROS \
    ENUM_OR_STRING(BOOT_PRIORITY_HIGHEST),      \
    ENUM_OR_STRING(BOOT_PRIORITY_HIGH),         \
    ENUM_OR_STRING(BOOT_PRIORITY_MEDIUM),       \
    ENUM_OR_STRING(BOOT_PRIORITY_LOW),          \
    ENUM_OR_STRING(BOOT_PRIORITY_LOWEST),       \
    ENUM_OR_STRING(BOOT_PRIORITY_BUTT)

enum EModuleBootPriority
{
    MODULE_BOOT_PRIORITY_MACROS
};

//---------------------------------------------------------------------------------------------------------------------
// - EStartupType
// --------------------------------------------------------------------------------------------------------------------
#define STARTUP_TYPE_MACROS \
    ENUM_OR_STRING(STARTUP_COLD_BOOT),  \
    ENUM_OR_STRING(STARTUP_WARM_BOOT),  \
    ENUM_OR_STRING(STARTUP_RESUME),     \
    ENUM_OR_STRING(STARTUP_BUTT)

enum EStartupType
{
    STARTUP_TYPE_MACROS
};

//---------------------------------------------------------------------------------------------------------------------
// - EPreStandbyAck
// --------------------------------------------------------------------------------------------------------------------
#define PRE_STANDBY_ACK_MACROS \
    ENUM_OR_STRING(PRE_STANDBY_ACK_ALLOW),  \
    ENUM_OR_STRING(PRE_STANDBY_ACK_REFUSE), \
    ENUM_OR_STRING(PRE_STANDBY_ACK_DELAY),  \
    ENUM_OR_STRING(PRE_STANDBY_ACK_BUTT)

enum EPreStandbyAck
{
    PRE_STANDBY_ACK_MACROS
};

//---------------------------------------------------------------------------------------------------------------------
// - EStandbySourceType
// --------------------------------------------------------------------------------------------------------------------
#define STANDBY_REASON_TYPE_MACROS \
    ENUM_OR_STRING(STANDBY_REASON_USER), \
    ENUM_OR_STRING(STANDBY_REASON_BUTT)

enum EStandbyReasonType
{
    STANDBY_REASON_TYPE_MACROS
};

//---------------------------------------------------------------------------------------------------------------------
// - EWakeupSourceType
// --------------------------------------------------------------------------------------------------------------------
#define WAKEUP_SOURCE_TYPE_MACROS \
    ENUM_OR_STRING(WAKEUP_SOURCE_USER), \
    ENUM_OR_STRING(WAKEUP_SOURCE_RTC),  \
    ENUM_OR_STRING(WAKEUP_SOURCE_BUTT)

enum EWakeupSourceType
{
    WAKEUP_SOURCE_TYPE_MACROS
};

//---------------------------------------------------------------------------------------------------------------------
// - ETimeSourceType
// --------------------------------------------------------------------------------------------------------------------
#define TIME_SOURCE_TYPE_MACROS \
    ENUM_OR_STRING(TIME_SOURCE_TYPE_MIN),   \
    ENUM_OR_STRING(TIME_SOURCE_TYPE_NTP),   \
    ENUM_OR_STRING(TIME_SOURCE_TYPE_GNSS),  \
    ENUM_OR_STRING(TIME_SOURCE_TYPE_BUTT)

enum TimeSourceType
{
    TIME_SOURCE_TYPE_MACROS
};

//---------------------------------------------------------------------------------------------------------------------
// - STimerInfo
// --------------------------------------------------------------------------------------------------------------------
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
    SignalType  sigId;
    void (ClassName::*callback)(const MsgType& msg);
};

} // namespace InternalDefs

#endif // __CORE_TYPE_DEFS_H__
