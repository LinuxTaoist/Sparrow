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
std::string GetSprErrorLevelText(int32_t level);
std::string GetSprErrorText(int32_t errorCode);

std::string GetSprProxTypeText(int32_t type);
std::string GetSprModuleIDText(int32_t id);
std::string GetSprProxyBinderCmdText(int32_t cmd);
std::string GetSprPropertyBinderCmdText(int32_t cmd);
std::string GetSprPowerMBinderCmdText(int32_t cmd);
std::string GetSprDebugMBinderCmdText(int32_t cmd);
std::string GetSprLogLevelText(int32_t level);
std::string GetSprGeneralBinderCmdText(int32_t cmd);
std::string GetSprModuleBootPriorityText(int32_t priority);
std::string GetStartupTypeText(int32_t type);
std::string GetSprPreStandbyAckText(int32_t standbyAck);
std::string GetStandbyReasonTypeText(int32_t reason);
std::string GetWakeupSourceTypeText(int32_t source);
std::string GetSprTimeSourceTypeText(int32_t source);

} // namespace InternalDefs

#endif
