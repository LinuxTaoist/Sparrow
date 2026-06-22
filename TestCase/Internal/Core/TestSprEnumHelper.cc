/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprEnumHelper.cc
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
#include <stdio.h>
#include "CoreTypeDefs.h"
#include "CommonErrorCodes.h"
#include "SprEnumHelper.h"
#include "gtest/gtest.h"

using namespace InternalDefs;

// --------------------------------------------------------------------------------------------------------------------
// - Test enums of CommonErrorCodes.h
// --------------------------------------------------------------------------------------------------------------------
TEST(Core_SprEnumHelper, GetSprErrorLevelText) {
    EXPECT_EQ(GetSprErrorLevelText(ERR_EVENT_LEVEL_UNKNOWN), "ERR_EVENT_LEVEL_UNKNOWN");
    EXPECT_EQ(GetSprErrorLevelText(ERR_EVENT_LEVEL_CRITICAL), "ERR_EVENT_LEVEL_CRITICAL");
    EXPECT_EQ(GetSprErrorLevelText(ERR_EVENT_LEVEL_ERROR), "ERR_EVENT_LEVEL_ERROR");
    EXPECT_EQ(GetSprErrorLevelText(ERR_EVENT_LEVEL_WARNNING), "ERR_EVENT_LEVEL_WARNNING");
    EXPECT_EQ(GetSprErrorLevelText(ERR_EVENT_LEVEL_INFO), "ERR_EVENT_LEVEL_INFO");
    EXPECT_EQ(GetSprErrorLevelText(ERR_EVENT_LEVEL_BUTT), "ERR_EVENT_LEVEL_BUTT");
    EXPECT_EQ(GetSprErrorLevelText(ERR_EVENT_LEVEL_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprErrorText) {
    EXPECT_EQ(GetSprErrorText(ERR_GENERAL_SUCCESS), "General success");
    EXPECT_EQ(GetSprErrorText(ERR_SHM_GET_SIZE_FAILED), "Failed to get shared memory size");
    EXPECT_EQ(GetSprErrorText(ERR_DEBUG_UNKNOW_ERROR), "Debug unknown error");
    EXPECT_EQ(GetSprErrorText(ERR_CODE_BUTT), "Unknown error code");
}

// --------------------------------------------------------------------------------------------------------------------
// - Test enums of CoreTypeDefs.h
// --------------------------------------------------------------------------------------------------------------------
TEST(Core_SprEnumHelper, GetSprProxTypeText) {
    EXPECT_EQ(GetSprProxTypeText(MEDIATOR_PROXY_MQUEUE), "MEDIATOR_PROXY_MQUEUE");
    EXPECT_EQ(GetSprProxTypeText(IPC_TYPE_BUTT), "IPC_TYPE_BUTT");
    EXPECT_EQ(GetSprProxTypeText(-1), "UNDEFINED");
    EXPECT_EQ(GetSprProxTypeText(IPC_TYPE_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprModuleIDText) {
    EXPECT_EQ(GetSprModuleIDText(MODULE_NONE), "MODULE_NONE");
    EXPECT_EQ(GetSprModuleIDText(MODULE_GENERAL), "MODULE_GENERAL");
    EXPECT_EQ(GetSprModuleIDText(MODULE_PUBLIC_END), "MODULE_PUBLIC_END");

    char negModuleExpected[32] = {};
    char overflowModuleExpected[32] = {};
    snprintf(negModuleExpected, sizeof(negModuleExpected), "MODULE_0X%X", static_cast<uint32_t>(-1));
    snprintf(overflowModuleExpected, sizeof(overflowModuleExpected), "MODULE_0X%X", static_cast<uint32_t>(MODULE_PUBLIC_END + 100));

    EXPECT_EQ(std::string(GetSprModuleIDText(-1)), std::string(negModuleExpected));
    EXPECT_EQ(std::string(GetSprModuleIDText(MODULE_PUBLIC_END + 100)), std::string(overflowModuleExpected));
}

TEST(Core_SprEnumHelper, GetSprProxyBinderCmdText) {
    EXPECT_EQ(GetSprProxyBinderCmdText(PROXY_CMD_GET_ALL_MQ_ATTRS), "PROXY_CMD_GET_ALL_MQ_ATTRS");
    EXPECT_EQ(GetSprProxyBinderCmdText(PROXY_CMD_BUTT), "PROXY_CMD_BUTT");
    EXPECT_EQ(GetSprProxyBinderCmdText(PROXY_CMD_GET_ALL_MQ_ATTRS - 10), "UNDEFINED");
    EXPECT_EQ(GetSprProxyBinderCmdText(PROXY_CMD_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprPropertyBinderCmdText) {
    EXPECT_EQ(GetSprPropertyBinderCmdText(PROPERTY_CMD_SET_PROPERTY), "PROPERTY_CMD_SET_PROPERTY");
    EXPECT_EQ(GetSprPropertyBinderCmdText(PROPERTY_CMD_BUTT), "PROPERTY_CMD_BUTT");
    EXPECT_EQ(GetSprPropertyBinderCmdText(PROPERTY_CMD_SET_PROPERTY - 10), "UNDEFINED");
    EXPECT_EQ(GetSprPropertyBinderCmdText(PROPERTY_CMD_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprPowerMBinderCmdText) {
    EXPECT_EQ(GetSprPowerMBinderCmdText(POWERM_CMD_POWER_ON), "POWERM_CMD_POWER_ON");
    EXPECT_EQ(GetSprPowerMBinderCmdText(POWERM_CMD_BUTT), "POWERM_CMD_BUTT");
    EXPECT_EQ(GetSprPowerMBinderCmdText(POWERM_CMD_POWER_ON - 10), "UNDEFINED");
    EXPECT_EQ(GetSprPowerMBinderCmdText(POWERM_CMD_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprDebugMBinderCmdText) {
    EXPECT_EQ(GetSprDebugMBinderCmdText(DEBUG_CMD_ENABLE_REMOTE_PORT), "DEBUG_CMD_ENABLE_REMOTE_PORT");
    EXPECT_EQ(GetSprDebugMBinderCmdText(DEBUG_MSG_BUTT), "DEBUG_MSG_BUTT");
    EXPECT_EQ(GetSprDebugMBinderCmdText(DEBUG_CMD_ENABLE_REMOTE_PORT - 10), "UNDEFINED");
    EXPECT_EQ(GetSprDebugMBinderCmdText(DEBUG_MSG_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprGeneralBinderCmdText) {
    EXPECT_EQ(GetSprGeneralBinderCmdText(GENERAL_CMD_EXE_EXIT), "GENERAL_CMD_EXE_EXIT");
    EXPECT_EQ(GetSprGeneralBinderCmdText(GENERAL_CMD_BUTT), "GENERAL_CMD_BUTT");
    EXPECT_EQ(GetSprGeneralBinderCmdText(GENERAL_CMD_EXE_EXIT - 20), "UNDEFINED");
    EXPECT_EQ(GetSprGeneralBinderCmdText(GENERAL_CMD_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprModuleBootPriorityText) {
    EXPECT_EQ(GetSprModuleBootPriorityText(BOOT_PRIORITY_HIGHEST), "BOOT_PRIORITY_HIGHEST");
    EXPECT_EQ(GetSprModuleBootPriorityText(BOOT_PRIORITY_LOWEST), "BOOT_PRIORITY_LOWEST");
    EXPECT_EQ(GetSprModuleBootPriorityText(BOOT_PRIORITY_HIGHEST - 20), "UNDEFINED");
    EXPECT_EQ(GetSprModuleBootPriorityText(BOOT_PRIORITY_LOWEST + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetStartupTypeText) {
    EXPECT_EQ(GetStartupTypeText(STARTUP_COLD_BOOT), "STARTUP_COLD_BOOT");
    EXPECT_EQ(GetStartupTypeText(STARTUP_WARM_BOOT), "STARTUP_WARM_BOOT");
    EXPECT_EQ(GetStartupTypeText(STARTUP_BUTT), "STARTUP_BUTT");
    EXPECT_EQ(GetStartupTypeText(STARTUP_COLD_BOOT - 20), "UNDEFINED");
    EXPECT_EQ(GetStartupTypeText(STARTUP_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprPreStandbyAckText) {
    EXPECT_EQ(GetSprPreStandbyAckText(PRE_STANDBY_ACK_ALLOW), "PRE_STANDBY_ACK_ALLOW");
    EXPECT_EQ(GetSprPreStandbyAckText(PRE_STANDBY_ACK_BUTT), "PRE_STANDBY_ACK_BUTT");
    EXPECT_EQ(GetSprPreStandbyAckText(PRE_STANDBY_ACK_ALLOW - 20), "UNDEFINED");
    EXPECT_EQ(GetSprPreStandbyAckText(PRE_STANDBY_ACK_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetStandbyReasonTypeText) {
    EXPECT_EQ(GetStandbyReasonTypeText(STANDBY_REASON_USER), "STANDBY_REASON_USER");
    EXPECT_EQ(GetStandbyReasonTypeText(STANDBY_REASON_BUTT), "STANDBY_REASON_BUTT");
    EXPECT_EQ(GetStandbyReasonTypeText(STANDBY_REASON_USER - 20), "UNDEFINED");
    EXPECT_EQ(GetStandbyReasonTypeText(STANDBY_REASON_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetWakeupSourceTypeText) {
    EXPECT_EQ(GetWakeupSourceTypeText(WAKEUP_SOURCE_USER), "WAKEUP_SOURCE_USER");
    EXPECT_EQ(GetWakeupSourceTypeText(WAKEUP_SOURCE_RTC), "WAKEUP_SOURCE_RTC");
    EXPECT_EQ(GetWakeupSourceTypeText(WAKEUP_SOURCE_BUTT), "WAKEUP_SOURCE_BUTT");
    EXPECT_EQ(GetWakeupSourceTypeText(WAKEUP_SOURCE_USER - 20), "UNDEFINED");
    EXPECT_EQ(GetWakeupSourceTypeText(WAKEUP_SOURCE_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprTimeSourceTypeText) {
    EXPECT_EQ(GetSprTimeSourceTypeText(TIME_SOURCE_TYPE_NTP), "TIME_SOURCE_TYPE_NTP");
    EXPECT_EQ(GetSprTimeSourceTypeText(TIME_SOURCE_TYPE_BUTT), "TIME_SOURCE_TYPE_BUTT");
    EXPECT_EQ(GetSprTimeSourceTypeText(TIME_SOURCE_TYPE_NTP - 20), "UNDEFINED");
    EXPECT_EQ(GetSprTimeSourceTypeText(TIME_SOURCE_TYPE_BUTT + 100), "UNDEFINED");
}
