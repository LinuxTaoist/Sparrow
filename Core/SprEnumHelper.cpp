/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CoreTypeDefs.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/03/15
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/03/15 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <vector>
#include "CoreTypeDefs.h"
#include "CommonErrorCodes.h"
#include "SprEnumHelper.h"

namespace InternalDefs {

#ifdef ENUM_OR_STRING
#undef ENUM_OR_STRING
#endif
#define ENUM_OR_STRING(x) #x

// --------------------------------------------------------------------------------------------------------------------
// - Enums of CommonErrorCodes.h
// --------------------------------------------------------------------------------------------------------------------
int32_t GetSprErrorLevel(int32_t errorCode)
{
    int32_t levelCode = (0 - errorCode) % 50;

    if (levelCode >= ERR_LEVEL_CRITICAL_BEGIN && levelCode < ERR_LEVEL_ERROR_BEGIN) {
        return ERR_EVENT_LEVEL_CRITICAL;
    }
    if (levelCode >= ERR_LEVEL_ERROR_BEGIN && levelCode < ERR_LEVEL_WARNNING_BEGIN) {
        return ERR_EVENT_LEVEL_ERROR;
    }
    if (levelCode >= ERR_LEVEL_WARNNING_BEGIN && levelCode < ERR_LEVEL_INFO_BEGIN) {
        return ERR_EVENT_LEVEL_WARNNING;
    }
    if (levelCode >= ERR_LEVEL_INFO_BEGIN){
        return ERR_EVENT_LEVEL_INFO;
    }

    return ERR_EVENT_LEVEL_UNKNOWN;

}

std::string GetSprErrorLevelDescription(int32_t level)
{
    std::vector<std::string> errorLevels = {
        ERR_EVENT_LEVEL_MACROS
    };

    return ((int32_t)errorLevels.size() > level) ? errorLevels[level] : "UNDEFINED";
}

std::string GetSprErrorDescription(int32_t errorCode)
{
    switch (errorCode) {
        case ERR_GENERAL_SUCCESS:             return "General success";
        case ERR_GENERAL_ERROR:               return "General error";
        case ERR_GENERAL_RUN_LONGTIME:        return "Run long time";
        case ERR_SYS_INVALID_PARAM:           return "Invalid parameter";
        case ERR_SYS_MEMORY_ALLOC_FAILED:     return "Memory allocation failed";
        case ERR_PARCEL_WAIT_FAILED:          return "Parcel wait failed";
        case ERR_PARCEL_WAIT_TIMEOUT:         return "Parcel wait timeout";
        case ERR_PARCEL_POST_FAILED:          return "Parcel post failed";
        case ERR_PARCE_WRITE_FAILED:          return "Parcel write failed";
        case ERR_PARCEL_READ_FAILED:          return "Parcel read failed";
        case ERR_BINDER_INIT_FAILED:          return "Binder initialization failed";
        case ERR_BINDER_INVALID_POINTER:      return "Binder invalid pointer";
        case ERR_SHM_CREATE_FAILED:           return "Shared memory creation failed";
        case ERR_SHM_OPEN_FAILED:             return "Shared memory open failed";
        case ERR_SHM_READ_FAILED:             return "Shared memory read failed";
        case ERR_SHM_WRITE_FAILED:            return "Shared memory write failed";
        case ERR_SHM_CLOSE_FAILED:            return "Shared memory close failed";
        case ERR_SHM_DESTROY_FAILED:          return "Shared memory destroy failed";
        case ERR_SHM_GET_SIZE_FAILED:         return "Failed to get shared memory size";
        case ERR_PROPERTY_UNKOWN_ERROR:       return "Property unknown error";
        case ERR_PROPERTY_BINDER_INIT_FAILED: return "Property binder initialization failed";
        case ERR_CONFIG_UNKOWN_ERROR:         return "Configuration unknown error";
        case ERR_LOGM_UNKOWN_ERROR:           return "Log manager unknown error";
        case ERR_TIMEM_UNKOWN_ERROR:          return "Time manager unknown error";
        case ERR_SYSTEM_TIMER_UNKOWN_ERROR:   return "System timer unknown error";
        case ERR_POWERM_UNKOWN_ERROR:         return "Power manager unknown error";
        case ERR_ONENET_MANAGER_UNKOWN_ERROR: return "OneNet manager unknown error";
        case ERR_ONENET_MANAGER_OFFLINE:      return "OneNet manager is offline";
        case ERR_ONENET_MANAGER_ONLINE:       return "OneNet manager is online";
        case ERR_ONENET_DEV_UNKOWN_ERROR:     return "OneNet device unknown error";
        case ERR_ONENET_DRIVER_UNKOWN_ERROR:  return "OneNet driver unknown error";
        case ERR_DEBUG_UNKNOW_ERROR:          return "Debug unknown error";
        default:                              return "Unknown error code";
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - Enums of CoreTypeDefs.h
// --------------------------------------------------------------------------------------------------------------------
std::string GetSprProxTypeDescription(int32_t type)
{
    std::vector<std::string> proxyTypes = {
        PROXY_TYPE_MACROS
    };

    return ((int32_t)proxyTypes.size() > type) ? proxyTypes[type] : "UNDEFINED";
}

std::string GetSprModuleIDDescription(int32_t moduleID)
{
    std::vector<std::string> moduleIDs = {
        SPR_MODULE_ID_MACROS
    };

    return ((int32_t)moduleIDs.size() > moduleID) ? moduleIDs[moduleID] : "UNDEFINED";
}

std::string GetSprProxyBinderCmdDescription(int32_t proxyBinderCmd)
{
    std::vector<std::string> proxyBinderCmds = {
        PROXY_BINDER_CMD_MACROS
    };

    int32_t index = proxyBinderCmd - PROXY_CMD_BEGIN - 1;
    return ((int32_t)proxyBinderCmds.size() > index) ? proxyBinderCmds[index] : "UNDEFINED";
}

std::string GetSprPropertyBinderCmdDescription(int32_t propertyBinderCmd)
{
    std::vector<std::string> propertyBinderCmds = {
        PROPERTY_BINDER_CMD_MACROS
    };

    int32_t index = propertyBinderCmd - PROPERTY_CMD_BEGIN - 1;
    return ((int32_t)propertyBinderCmds.size() > index) ? propertyBinderCmds[index] : "UNDEFINED";
}

std::string GetSprPowerMBinderCmdDescription(int32_t powerMBinderCmd)
{
    std::vector<std::string> powerMBinderCmds = {
        POWER_BINDER_CMD_MACROS
    };

    int32_t index = powerMBinderCmd - POWERM_CMD_BEGIN - 1;
    return ((int32_t)powerMBinderCmds.size() > index) ? powerMBinderCmds[index] : "UNDEFINED";
}

std::string GetSprDebugMBinderCmdDescription(int32_t debugMBinderCmd)
{
    std::vector<std::string> debugMBinderCmds = {
        DEBUG_BINDER_CMD_MACROS
    };

    int32_t index = debugMBinderCmd - DEBUG_CMD_BEGIN - 1;
    return ((int32_t)debugMBinderCmds.size() > index) ? debugMBinderCmds[index] : "UNDEFINED";
}

std::string GetSprOneNetBinderCmdDescription(int32_t oneNetBinderCmd)
{
    std::vector<std::string> oneNetBinderCmds = {
        ONENET_BINDER_CMD_MACROS
    };

    int32_t index = oneNetBinderCmd - ONENET_CMD_BEGIN - 1;
    return ((int32_t)oneNetBinderCmds.size() > index) ? oneNetBinderCmds[index] : "UNDEFINED";
}

std::string GetSprGeneralBinderCmdDescription(int32_t generalBinderCmd)
{
    std::vector<std::string> generalBinderCmds = {
        GENERAL_BINDER_CMD_MACROS
    };

    int32_t index = generalBinderCmd - GENERAL_CMD_BEGIN - 1;
    return ((int32_t)generalBinderCmds.size() > index) ? generalBinderCmds[index] : "UNDEFINED";
}

std::string GetSprModuleBootPriorityDescription(int32_t moduleBootPriority)
{
    std::vector<std::string> moduleBootPriorities = {
        MODULE_BOOT_PRIORITY_MACROS
    };

    return ((int32_t)moduleBootPriorities.size() > moduleBootPriority) ? moduleBootPriorities[moduleBootPriority] : "UNDEFINED";
}

std::string GetSprTimeSourceTypeDescription(int32_t timeSourceType)
{
    std::vector<std::string> timeSourceTypes = {
        TIME_SOURCE_TYPE_MACROS
    };

    return ((int32_t)timeSourceTypes.size() > timeSourceType) ? timeSourceTypes[timeSourceType] : "UNDEFINED";
}

} // namespace InternalDefs
