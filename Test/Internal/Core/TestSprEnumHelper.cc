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
#include "CommonErrorCodes.h"
#include "SprEnumHelper.h"
#include "gtest/gtest.h"

TEST(Core_SprEnumHelper, GetSprProxTypeDescription) {
    EXPECT_EQ(InternalDefs::GetSprProxTypeDescription(InternalDefs::MEDIATOR_PROXY_MQUEUE), "MEDIATOR_PROXY_MQUEUE");
    EXPECT_EQ(InternalDefs::GetSprProxTypeDescription(InternalDefs::IPC_TYPE_BUTT), "IPC_TYPE_BUTT");
    EXPECT_EQ(InternalDefs::GetSprProxTypeDescription(100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprModuleIDDescription) {
    EXPECT_EQ(InternalDefs::GetSprModuleIDDescription(InternalDefs::MODULE_NONE), "MODULE_NONE");
    EXPECT_EQ(InternalDefs::GetSprModuleIDDescription(InternalDefs::MODULE_GENERAL), "MODULE_GENERAL");
    EXPECT_EQ(InternalDefs::GetSprModuleIDDescription(InternalDefs::MODULE_MAX), "MODULE_MAX");
    EXPECT_EQ(InternalDefs::GetSprModuleIDDescription(100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprProxyBinderCmdDescription) {
    EXPECT_EQ(InternalDefs::GetSprProxyBinderCmdDescription(InternalDefs::PROXY_CMD_GET_ALL_MQ_ATTRS), "PROXY_CMD_GET_ALL_MQ_ATTRS");
    EXPECT_EQ(InternalDefs::GetSprProxyBinderCmdDescription(InternalDefs::PROXY_CMD_BUTT), "PROXY_CMD_BUTT");
    EXPECT_EQ(InternalDefs::GetSprProxyBinderCmdDescription(InternalDefs::PROXY_CMD_BEGIN + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprPropertyBinderCmdDescription) {
    EXPECT_EQ(InternalDefs::GetSprPropertyBinderCmdDescription(InternalDefs::PROPERTY_CMD_SET_PROPERTY), "PROPERTY_CMD_SET_PROPERTY");
    EXPECT_EQ(InternalDefs::GetSprPropertyBinderCmdDescription(InternalDefs::PROPERTY_CMD_BUTT), "PROPERTY_CMD_BUTT");
    EXPECT_EQ(InternalDefs::GetSprPropertyBinderCmdDescription(InternalDefs::PROPERTY_CMD_BEGIN + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprPowerMBinderCmdDescription) {
    EXPECT_EQ(InternalDefs::GetSprPowerMBinderCmdDescription(InternalDefs::POWERM_CMD_POWER_ON), "POWERM_CMD_POWER_ON");
    EXPECT_EQ(InternalDefs::GetSprPowerMBinderCmdDescription(InternalDefs::POWERM_CMD_BUTT), "POWERM_CMD_BUTT");
    EXPECT_EQ(InternalDefs::GetSprPowerMBinderCmdDescription(InternalDefs::POWERM_CMD_BEGIN + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprDebugMBinderCmdDescription) {
    EXPECT_EQ(InternalDefs::GetSprDebugMBinderCmdDescription(InternalDefs::DEBUG_CMD_ENABLE_REMOTE_PORT), "DEBUG_CMD_ENABLE_REMOTE_PORT");
    EXPECT_EQ(InternalDefs::GetSprDebugMBinderCmdDescription(InternalDefs::DEBUG_MSG_BUTT), "DEBUG_MSG_BUTT");
    EXPECT_EQ(InternalDefs::GetSprDebugMBinderCmdDescription(InternalDefs::DEBUG_CMD_BEGIN + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprOneNetBinderCmdDescription) {
    EXPECT_EQ(InternalDefs::GetSprOneNetBinderCmdDescription(InternalDefs::ONENET_CMD_ACTIVE_DEVICE), "ONENET_CMD_ACTIVE_DEVICE");
    EXPECT_EQ(InternalDefs::GetSprOneNetBinderCmdDescription(InternalDefs::ONENET_CMD_BUTT), "ONENET_CMD_BUTT");
    EXPECT_EQ(InternalDefs::GetSprOneNetBinderCmdDescription(InternalDefs::ONENET_CMD_BEGIN + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprGeneralBinderCmdDescription) {
    EXPECT_EQ(InternalDefs::GetSprGeneralBinderCmdDescription(InternalDefs::GENERAL_CMD_EXE_EXIT), "GENERAL_CMD_EXE_EXIT");
    EXPECT_EQ(InternalDefs::GetSprGeneralBinderCmdDescription(InternalDefs::GENERAL_CMD_BUTT), "GENERAL_CMD_BUTT");
    EXPECT_EQ(InternalDefs::GetSprGeneralBinderCmdDescription(InternalDefs::GENERAL_CMD_BEGIN + 100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprModuleBootPriorityDescription) {
    EXPECT_EQ(InternalDefs::GetSprModuleBootPriorityDescription(InternalDefs::BOOT_PRIORITY_HIGHEST), "BOOT_PRIORITY_HIGHEST");
    EXPECT_EQ(InternalDefs::GetSprModuleBootPriorityDescription(InternalDefs::BOOT_PRIORITY_LOWEST), "BOOT_PRIORITY_LOWEST");
    EXPECT_EQ(InternalDefs::GetSprModuleBootPriorityDescription(100), "UNDEFINED");
}

TEST(Core_SprEnumHelper, GetSprTimeSourceTypeDescription) {
    EXPECT_EQ(InternalDefs::GetSprTimeSourceTypeDescription(InternalDefs::TIME_SOURCE_TYPE_NTP), "TIME_SOURCE_TYPE_NTP");
    EXPECT_EQ(InternalDefs::GetSprTimeSourceTypeDescription(InternalDefs::TIME_SOURCE_TYPE_BUTT), "TIME_SOURCE_TYPE_BUTT");
    EXPECT_EQ(InternalDefs::GetSprTimeSourceTypeDescription(100), "UNDEFINED");
}

TEST(ErrorDescriptionTest, GetSprErrorDescription) {
    EXPECT_EQ(InternalDefs::GetSprErrorDescription(CommonErrorCode::ERR_GENERAL_SUCCESS), "General success");
    EXPECT_EQ(InternalDefs::GetSprErrorDescription(CommonErrorCode::ERR_DEBUG_UNKNOW_ERROR), "Debug unknown error");
    EXPECT_EQ(InternalDefs::GetSprErrorDescription(CommonErrorCode::ERR_CODE_BUTT), "Unknown error code");
}
