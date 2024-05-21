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

#endif // __COMMON_MACROS_H__
