/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprSigId.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SprSigId 信号名映射内部测试
 *  @date       : 2026/09/09
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/09 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <string>
#include <cstdio>
#include "SprSigId.h"
#include "gtest/gtest.h"

// 测试已定义信号 ID 返回稳定的名称
TEST(Core_SprSigId, KnownSignalIdsReturnStableNames)
{
    EXPECT_STREQ("SIG_ID_ANY", GetSigName(InternalDefs::SIG_ID_ANY));
    EXPECT_STREQ("SIG_ID_PROXY_REGISTER_REQUEST", GetSigName(InternalDefs::SIG_ID_PROXY_REGISTER_REQUEST));
    EXPECT_STREQ("SIG_ID_TEST_MODULE_5S_TIMER_EVENT", GetSigName(InternalDefs::SIG_ID_TEST_MODULE_5S_TIMER_EVENT));
}

// 测试未定义信号 ID 返回十六进制回退文本
TEST(Core_SprSigId, UnknownSignalIdReturnsHexFallback)
{
    const uint32_t unknownId = InternalDefs::SIG_ID_PUBLIC_END + 0x55U;
    char expected[64] = {};
    std::snprintf(expected, sizeof(expected), "SIGID_0X%X", unknownId);

    EXPECT_STREQ(expected, GetSigName(unknownId));
}

// 测试 SIG_ID_PUBLIC_END 边界使用回退文本
TEST(Core_SprSigId, PublicEndBoundaryUsesFallbackText)
{
    const uint32_t boundaryId = InternalDefs::SIG_ID_PUBLIC_END;
    char expected[64] = {};
    std::snprintf(expected, sizeof(expected), "SIGID_0X%X", boundaryId);

    EXPECT_STREQ(expected, GetSigName(boundaryId));
}
