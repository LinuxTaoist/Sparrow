/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprPropertyMacros.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/03/08
 *
 *  Defines macros for property names across all general modules..
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/03/08 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_PROPERTY_MACROS_H__
#define __SPR_PROPERTY_MACROS_H__

// Naming rules: [attribute].[namespace].[module/component].[function/feature].[description]
// attribute:
//      ro      - read only
//      persist - persistent
// * namespace: means defined by files
//      vendor
//      system
//      ...
// * module:
//      wifi
//      log
//      ...
// * function/feature:
//      signal
//      level
//      ...
// description: ...
// Note: Mark with '*' means must be defined
// Example:
//   ro.system.software.version
//   vendor.audio.volume
//-----------------------------------------------------------------------------
// general
//-----------------------------------------------------------------------------

// system.prop
#define PROP_SOFTWARE_VERSION       "ro.system.software.version"
#define PROPERTY_KEY_LOG_LEVEL      "system.log.level"

// others, not defined in propery files

//-----------------------------------------------------------------------------
// vendor
//-----------------------------------------------------------------------------

// vendor.prop
#define PROP_VENDOR                 "ro.vendor.platform.name"

#endif // __SPR_PROPERTY_MACROS_H__
