/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CommonErrorCodes.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/02/10
 *
 *  This header file defines common error codes for both client and server code implementations.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/02/10 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __COMMON_ERROR_CODES_H__
#define __COMMON_ERROR_CODES_H__

// Define the starting points for different error levels.
// Each module is allocated 50 error codes, which are divided into the following severity levels:
// -01 ~ -09: Critical errors (severe issues that may cause system failure or data loss)
// -10 ~ -29: Error level (issues that need immediate attention but do not cause system failure)
// -30 ~ -39: Warning level (issues that should be addressed but do not require immediate action)
// -40 ~ -49: Info level (informational messages or minor issues that do not affect system operation)
#define ERR_LEVEL_CRITICAL_BEGIN    1
#define ERR_LEVEL_ERROR_BEGIN       10
#define ERR_LEVEL_WARNNING_BEGIN    30
#define ERR_LEVEL_INFO_BEGIN        40

namespace {

#ifdef ENUM_OR_STRING
#undef ENUM_OR_STRING
#endif
#define ENUM_OR_STRING(x) x

}

#define ERR_EVENT_LEVEL_MACROS \
    ENUM_OR_STRING(ERR_EVENT_LEVEL_UNKNOWN),    \
    ENUM_OR_STRING(ERR_EVENT_LEVEL_CRITICAL),   \
    ENUM_OR_STRING(ERR_EVENT_LEVEL_ERROR),      \
    ENUM_OR_STRING(ERR_EVENT_LEVEL_WARNNING),   \
    ENUM_OR_STRING(ERR_EVENT_LEVEL_INFO),       \
    ENUM_OR_STRING(ERR_EVENT_LEVEL_BUTT)

enum SprErrorLevel
{
    ERR_EVENT_LEVEL_MACROS
};

enum CommonErrorCode {
    // general error code
    ERR_GENERAL_SUCCESS             =  0,
    ERR_GENERAL_ERROR               = -1,
    ERR_GENERAL_RUN_LONGTIME        = -30,

    // system error code
    ERR_SYS_INVALID_PARAM           = -310,
    ERR_SYS_MEMORY_ALLOC_FAILED     = -311,
    ERR_PARCEL_WAIT_FAILED          = -312,
    ERR_PARCEL_WAIT_TIMEOUT         = -303,
    ERR_PARCEL_POST_FAILED          = -314,
    ERR_PARCE_WRITE_FAILED          = -315,
    ERR_PARCEL_READ_FAILED          = -316,
    ERR_BINDER_INIT_FAILED          = -317,
    ERR_BINDER_INVALID_POINTER      = -318,
    ERR_SHM_CREATE_FAILED           = -319,
    ERR_SHM_OPEN_FAILED             = -320,
    ERR_SHM_READ_FAILED             = -321,
    ERR_SHM_WRITE_FAILED            = -322,
    ERR_SHM_CLOSE_FAILED            = -323,
    ERR_SHM_DESTROY_FAILED          = -324,
    ERR_SHM_GET_SIZE_FAILED         = -325,

    ERR_PROPERTY_UNKOWN_ERROR       = -501,
    ERR_PROPERTY_BINDER_INIT_FAILED = -510,

    ERR_CONFIG_UNKOWN_ERROR         = -551,

    ERR_LOGM_UNKOWN_ERROR           = -601,

    ERR_TIMEM_UNKOWN_ERROR          = -651,

    ERR_SYSTEM_TIMER_UNKOWN_ERROR   = -701,

    ERR_POWERM_UNKOWN_ERROR         = -751,
    ERR_POWERM_REFUSE_STANDBY       = -780,
    ERR_POWERM_DELAY_STANDBY        = -781,

    ERR_DEBUG_UNKNOW_ERROR          = -951,

    ERR_CODE_BUTT
};

#endif // __COMMON_ERROR_CODES_H__
