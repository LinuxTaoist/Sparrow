/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprProcPrepare.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SprProcPrepare 进程框架初始化内部测试
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
#include <unistd.h>
#include <string>
#include "SprProcPrepare.h"
#include "gtest/gtest.h"

// 测试单例模式返回非空
TEST(Core_SprProcPrepare, SingletonReturnsValid)
{
    SprProcPrepare* a = SprProcPrepare::GetInstance();
    SprProcPrepare* b = SprProcPrepare::GetInstance();
    ASSERT_TRUE(a != nullptr);
    EXPECT_EQ(a, b);
}

// 测试进程框架初始化（初始化 proc info 与 debug pipe node）
TEST(Core_SprProcPrepare, Init)
{
    SprProcPrepare* prepare = SprProcPrepare::GetInstance();
    ASSERT_TRUE(prepare != nullptr);

    const std::string procName = "spr_proc_prepare_test";
    unlink(("/tmp/" + procName).c_str());

    EXPECT_EQ(prepare->Init(procName), 0);

    // 清理 Init 创建的 debug pipe 文件
    unlink(("/tmp/" + procName).c_str());
}
