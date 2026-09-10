/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprContext.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SprContext 上下文对象内部测试
 *  @date       : 2026/09/10
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/10 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "SprContext.h"
#include "gtest/gtest.h"

// 测试上下文对象构造与析构
TEST(Core_SprContext, ConstructAndDestruct)
{
    SprContext ctx;
    SUCCEED();
}

// 测试栈上多次创建上下文对象
TEST(Core_SprContext, MultipleInstances)
{
    SprContext ctx1;
    SprContext ctx2;
    SprContext ctx3;
    SUCCEED();
}
