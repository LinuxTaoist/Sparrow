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
#ifndef __SPR_ENUM_HELPER_H__
#define __SPR_ENUM_HELPER_H__

#include <string>

namespace InternalDefs {

int32_t GetSprErrorLevel(int32_t errorCode);
std::string GetSprErrorLevelDescription(int32_t level);
std::string GetSprErrorDescription(int32_t errorCode);

std::string GetSprProxTypeDescription(int32_t type);
std::string GetSprModuleIDDescription(int32_t id);
std::string GetSprProxTypeDescription(int32_t type);
std::string GetSprModuleIDDescription(int32_t id);
std::string GetSprProxyBinderCmdDescription(int32_t cmd);
std::string GetSprPropertyBinderCmdDescription(int32_t cmd);
std::string GetSprPowerMBinderCmdDescription(int32_t cmd);
std::string GetSprDebugMBinderCmdDescription(int32_t cmd);
std::string GetSprOneNetBinderCmdDescription(int32_t cmd);
std::string GetSprGeneralBinderCmdDescription(int32_t cmd);
std::string GetSprModuleBootPriorityDescription(int32_t priority);
std::string GetSprTimeSourceTypeDescription(int32_t source);

} // namespace InternalDefs

#endif
