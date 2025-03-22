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
#include "gtest/gtest.h"

using namespace InternalDefs;

// --------------------------------------------------------------------------------------------------------------------
// - Test enums of CommonErrorCodes.h
// --------------------------------------------------------------------------------------------------------------------
TEST(Core_SprEnumHelper, GetSprErrorLevelDescription) {
    EXPECT_EQ(GetSprErrorLevelDescription(ERR_EVENT_LEVEL_UNKNOWN), "ERR_EVENT_LEVEL_UNKNOWN");
    EXPECT_EQ(GetSprErrorLevelDescription(ERR_EVENT_LEVEL_CRITICAL), "ERR_EVENT_LEVEL_CRITICAL");
    EXPECT_EQ(GetSprErrorLevelDescription(ERR_EVENT_LEVEL_ERROR), "ERR_EVENT_LEVEL_ERROR");
    EXPECT_EQ(GetSprErrorLevelDescription(ERR_EVENT_LEVEL_WARNNING), "ERR_EVENT_LEVEL_WARNNING");
    EXPECT_EQ(GetSprErrorLevelDescription(ERR_EVENT_LEVEL_INFO), "ERR_EVENT_LEVEL_INFO");
    EXPECT_EQ(GetSprErrorLevelDescription(ERR_EVENT_LEVEL_BUTT), "ERR_EVENT_LEVEL_BUTT");
    EXPECT_EQ(GetSprErrorLevelDescription(ERR_EVENT_LEVEL_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprErrorDescription) {
    EXPECT_EQ(GetSprErrorDescription(ERR_GENERAL_SUCCESS), "General success");
    EXPECT_EQ(GetSprErrorDescription(ERR_SHM_GET_SIZE_FAILED), "Failed to get shared memory size");
    EXPECT_EQ(GetSprErrorDescription(ERR_DEBUG_UNKNOW_ERROR), "Debug unknown error");
    EXPECT_EQ(GetSprErrorDescription(ERR_CODE_BUTT), "Unknown error code");
}

// --------------------------------------------------------------------------------------------------------------------
// - Test enums of CoreTypeDefs.h
// --------------------------------------------------------------------------------------------------------------------
TEST(Core_SprEnumHelper, GetSprProxTypeDescription) {
    EXPECT_EQ(GetSprProxTypeDescription(MEDIATOR_PROXY_MQUEUE), "MEDIATOR_PROXY_MQUEUE");
    EXPECT_EQ(GetSprProxTypeDescription(IPC_TYPE_BUTT), "IPC_TYPE_BUTT");
    EXPECT_EQ(GetSprProxTypeDescription(IPC_TYPE_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprModuleIDDescription) {
    EXPECT_EQ(GetSprModuleIDDescription(MODULE_NONE), "MODULE_NONE");
    EXPECT_EQ(GetSprModuleIDDescription(MODULE_GENERAL), "MODULE_GENERAL");
    EXPECT_EQ(GetSprModuleIDDescription(MODULE_MAX), "MODULE_MAX");
    EXPECT_EQ(GetSprModuleIDDescription(MODULE_MAX + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprProxyBinderCmdDescription) {
    EXPECT_EQ(GetSprProxyBinderCmdDescription(PROXY_CMD_GET_ALL_MQ_ATTRS), "PROXY_CMD_GET_ALL_MQ_ATTRS");
    EXPECT_EQ(GetSprProxyBinderCmdDescription(PROXY_CMD_BUTT), "PROXY_CMD_BUTT");
    EXPECT_EQ(GetSprProxyBinderCmdDescription(PROXY_CMD_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprPropertyBinderCmdDescription) {
    EXPECT_EQ(GetSprPropertyBinderCmdDescription(PROPERTY_CMD_SET_PROPERTY), "PROPERTY_CMD_SET_PROPERTY");
    EXPECT_EQ(GetSprPropertyBinderCmdDescription(PROPERTY_CMD_BUTT), "PROPERTY_CMD_BUTT");
    EXPECT_EQ(GetSprPropertyBinderCmdDescription(PROPERTY_CMD_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprPowerMBinderCmdDescription) {
    EXPECT_EQ(GetSprPowerMBinderCmdDescription(POWERM_CMD_POWER_ON), "POWERM_CMD_POWER_ON");
    EXPECT_EQ(GetSprPowerMBinderCmdDescription(POWERM_CMD_BUTT), "POWERM_CMD_BUTT");
    EXPECT_EQ(GetSprPowerMBinderCmdDescription(POWERM_CMD_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprDebugMBinderCmdDescription) {
    EXPECT_EQ(GetSprDebugMBinderCmdDescription(DEBUG_CMD_ENABLE_REMOTE_PORT), "DEBUG_CMD_ENABLE_REMOTE_PORT");
    EXPECT_EQ(GetSprDebugMBinderCmdDescription(DEBUG_MSG_BUTT), "DEBUG_MSG_BUTT");
    EXPECT_EQ(GetSprDebugMBinderCmdDescription(DEBUG_MSG_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprOneNetBinderCmdDescription) {
    EXPECT_EQ(GetSprOneNetBinderCmdDescription(ONENET_CMD_ACTIVE_DEVICE), "ONENET_CMD_ACTIVE_DEVICE");
    EXPECT_EQ(GetSprOneNetBinderCmdDescription(ONENET_CMD_BUTT), "ONENET_CMD_BUTT");
    EXPECT_EQ(GetSprOneNetBinderCmdDescription(ONENET_CMD_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprGeneralBinderCmdDescription) {
    EXPECT_EQ(GetSprGeneralBinderCmdDescription(GENERAL_CMD_EXE_EXIT), "GENERAL_CMD_EXE_EXIT");
    EXPECT_EQ(GetSprGeneralBinderCmdDescription(GENERAL_CMD_BUTT), "GENERAL_CMD_BUTT");
    EXPECT_EQ(GetSprGeneralBinderCmdDescription(GENERAL_CMD_BUTT + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprModuleBootPriorityDescription) {
    EXPECT_EQ(GetSprModuleBootPriorityDescription(BOOT_PRIORITY_HIGHEST), "BOOT_PRIORITY_HIGHEST");
    EXPECT_EQ(GetSprModuleBootPriorityDescription(BOOT_PRIORITY_LOWEST), "BOOT_PRIORITY_LOWEST");
    EXPECT_EQ(GetSprModuleBootPriorityDescription(BOOT_PRIORITY_LOWEST + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprTimeSourceTypeDescription) {
    EXPECT_EQ(GetSprTimeSourceTypeDescription(TIME_SOURCE_TYPE_NTP), "TIME_SOURCE_TYPE_NTP");
    EXPECT_EQ(GetSprTimeSourceTypeDescription(TIME_SOURCE_TYPE_BUTT), "TIME_SOURCE_TYPE_BUTT");
    EXPECT_EQ(GetSprTimeSourceTypeDescription(TIME_SOURCE_TYPE_BUTT + 100), "UNDEFINED");
}

