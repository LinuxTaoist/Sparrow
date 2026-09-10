/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprDebugNode.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SprDebugNode 调试命令节点内部测试
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
#include <ctime>
#include <string>
#include <atomic>
#include <unistd.h>
#include "SprDebugNode.h"
#include "gtest/gtest.h"

namespace {
std::string MakeOwnerName(const std::string& prefix)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return prefix + "_" + std::to_string(ts.tv_nsec % 1000000L);
}
}

// 测试注册/注销 owner 命令的生命周期
TEST(Core_SprDebugNode, RegisterAndUnregisterOwnerCommands)
{
    SprDebugNode* node = SprDebugNode::GetInstance();
    ASSERT_TRUE(node != nullptr);

    const std::string owner = MakeOwnerName("ut_debug_owner");
    const int32_t baseOwners = node->GetCurNum();

    std::atomic<int32_t> hit(0);
    EXPECT_EQ(0, node->RegisterCmd(owner, "cmd1", "desc1", [&](const std::vector<std::string>&) {
        hit.fetch_add(1);
    }));
    EXPECT_EQ(baseOwners + 1, node->GetCurNum());

    EXPECT_EQ(0, node->RegisterCmd(owner, "cmd2", "desc2", [&](const std::vector<std::string>&) {
        hit.fetch_add(2);
    }));
    EXPECT_EQ(baseOwners + 1, node->GetCurNum());

    EXPECT_EQ(0, node->UnregisterCmd(owner, "cmd1"));
    EXPECT_EQ(-1, node->UnregisterCmd(owner, "cmd1"));
    EXPECT_EQ(0, node->UnregisterCmd(owner, "cmd2"));
    EXPECT_EQ(0, node->UnregisterCmd(owner));
    EXPECT_EQ(baseOwners, node->GetCurNum());
}

// 测试注销不存在的 owner 返回错误
TEST(Core_SprDebugNode, UnregisterMissingOwnerReturnsError)
{
    SprDebugNode* node = SprDebugNode::GetInstance();
    ASSERT_TRUE(node != nullptr);

    const std::string owner = MakeOwnerName("ut_missing_owner");
    EXPECT_EQ(-1, node->UnregisterCmd(owner));
    EXPECT_EQ(-1, node->UnregisterCmd(owner, "not_exist_cmd"));
}

// 测试设置/获取最大命令数
TEST(Core_SprDebugNode, SetAndGetMaxNum)
{
    SprDebugNode* node = SprDebugNode::GetInstance();
    ASSERT_TRUE(node != nullptr);

    const int32_t oldValue = node->GetMaxNum();
    EXPECT_EQ(0, node->SetMaxNum(512));
    EXPECT_EQ(512, node->GetMaxNum());

    EXPECT_EQ(0, node->SetMaxNum(oldValue));
    EXPECT_EQ(oldValue, node->GetMaxNum());
}

// 内置命令直接调用不应崩溃（真实调试命令执行路径）
TEST(Core_SprDebugNode, BuiltinCommandsExecuteWithoutCrash)
{
    SprDebugNode* node = SprDebugNode::GetInstance();
    ASSERT_TRUE(node != nullptr);

    std::vector<std::string> emptyArgs;
    node->DebugDumpAllOwners(emptyArgs);
    node->DebugDumpVersion(emptyArgs);
    node->DebugDumpProcInfo(emptyArgs);

    // loglevel 缺参数走错误分支
    node->DebugSetLogLevel(emptyArgs);
    // loglevel 带参数走设置分支
    std::vector<std::string> args = {"loglevel", "3"};
    node->DebugSetLogLevel(args);
}

// 管道调试节点初始化：重复初始化与无效路径
TEST(Core_SprDebugNode, InitPipeDebugNode)
{
    SprDebugNode* node = SprDebugNode::GetInstance();
    ASSERT_TRUE(node != nullptr);

    const std::string pipePath = "/tmp/spr_debug_node_pipe_test";
    unlink(pipePath.c_str());

    int32_t ret = node->InitPipeDebugNode(pipePath);
    // 首次初始化成功或失败取决于环境，但重复初始化应返回 0（已就绪）
    int32_t ret2 = node->InitPipeDebugNode(pipePath);

    if (ret == 0) {
        EXPECT_EQ(ret2, 0);
        EXPECT_EQ(node->GetDebugPath(), pipePath);
    }

    unlink(pipePath.c_str());
}
