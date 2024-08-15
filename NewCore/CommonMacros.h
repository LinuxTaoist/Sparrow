/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CommonMacros.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/03/02
 *
 *  This header is used to define macro definitions common to both client and server code implementations.
 *  The implementation of this header file is not publicly available
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/02 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __COMMON_MACROS_H__
#define __COMMON_MACROS_H__

// --------------------------------------------------------------------------------------------------------------------
// - Common macros for CommonMacros.h version
// --------------------------------------------------------------------------------------------------------------------
#define COMMON_MACROS_VERSION   "COMMON_MACROS_VERSION_M1001"

// --------------------------------------------------------------------------------------------------------------------
// - Common macros for build information
// --------------------------------------------------------------------------------------------------------------------
#ifndef PROJECT_INFO
    #define PROJECT_INFO    "Unknown"
#endif

#ifndef CXX_STANDARD
    #define CXX_STANDARD    "Unknown"
#endif

#ifndef GXX_VERSION
    #define GXX_VERSION     "Unknown"
#endif

#ifndef GCC_VERSION
    #define GCC_VERSION     "Unknown"
#endif

#ifndef RUN_ENV
    #define RUN_ENV         "Unknown"
#endif

#ifndef BUILD_TIME
    #define BUILD_TIME      "Unknown"
#endif

#ifndef BUILD_TYPE
    #define BUILD_TYPE      "Unknown"
#endif

#ifndef BUILD_HOST
    #define BUILD_HOST      "Unknown"
#endif

#ifndef BUILD_PLATFORM
    #define BUILD_PLATFORM  "Unknown"
#endif

// --------------------------------------------------------------------------------------------------------------------
// - Common macros for service and client
// --------------------------------------------------------------------------------------------------------------------
#define LOG_CACHE_MEMORY_SIZE       10 * 1024 * 1024    // 10MB
#define LOG_CACHE_MEMORY_PATH       "/tmp/SprLogShm"

// --------------------------------------------------------------------------------------------------------------------
// - Service name macro
// --------------------------------------------------------------------------------------------------------------------
#define SRV_NAME_BINDER             "bindermanagersrv"
#define SRV_NAME_PROPERTY           "propertiessrv"
#define SRV_NAME_MEDIATOR           "mediatorsrv"
#define SRV_NAME_LOG                "logmanagersrv"
#define SRV_NAME_SPARROW            "sparrowsrv"
#define SRV_NAME_POWER_MANAGER      "powermanagersrv"
#define SRV_NAME_DEBUG_MODULE       "debugsrv"

#endif // __COMMON_MACROS_H__
