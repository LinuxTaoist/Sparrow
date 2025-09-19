/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSharedBinaryTree.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.1
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/03/17
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/03/16 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <map>
#include <memory>
#include <stdio.h>
#include "gtest/gtest.h"
#include "SharedBinaryTree.h"

#define SHARED_BTREE_MAX_SIZE 10240

#define TEST_LOG(fmt, args...) printf("[   INFO   ] " fmt "\n", ##args)

class Util_SharedBinaryTree : public ::testing::Test {
protected:
    void SetUp() override {
        mpTree = std::make_shared<SharedBinaryTree>("/tmp/test_shared_tree", SHARED_BTREE_MAX_SIZE, false);
        ASSERT_EQ(true, (mpTree != nullptr));
        SUCCEED() << "Node is " << sizeof(Node) << " bytes.";
    }

    void TearDown() override {
        unlink("/tmp/test_shared_tree");
    }

protected:
    std::shared_ptr<SharedBinaryTree> mpTree;
};

// 测试 SetValue 和 GetValue 功能
TEST_F(Util_SharedBinaryTree, TestSetAndGetValue) {
    std::string expectKey = "test_key";
    std::string expectValue = "test_value";

    int sRet = mpTree->SetValue(expectKey, expectValue);
    EXPECT_EQ(0, sRet);

    std::string actualValue;
    int gRet = mpTree->GetValue(expectKey, actualValue);
    EXPECT_EQ(0, gRet);
    EXPECT_EQ(expectValue, actualValue);
}

// 测试 GetAllKeyValues 功能
TEST_F(Util_SharedBinaryTree, TestGetAllKeyValues) {
    std::map<std::string, std::string> expectMap = {
        {"key1", "value1"},
        {"key2", "value2"},
        {"key3", "value3"},
    };

    for (const auto& pair : expectMap) {
        int ret = mpTree->SetValue(pair.first, pair.second);
        EXPECT_EQ(0, ret);
    }

    std::map<std::string, std::string> actualMap;
    mpTree->GetAllKeyValues(actualMap);
    for (const auto& pair : expectMap) {
        auto actual = actualMap.find(pair.first);
        EXPECT_EQ(true, (actual != actualMap.end()));

        if (actual != actualMap.end()) {
            EXPECT_EQ(pair.first, actual->first);
            EXPECT_EQ(pair.second, actual->second);
        }
    }
}

// 测试键长度超出限制的情况
TEST_F(Util_SharedBinaryTree, TestKeyLengthLimit) {
    std::string longKey(SHARED_BTREE_KEY_MAX_LEN, 'a');
    std::string value = "key_len_limit_value";

    int sRet = mpTree->SetValue(longKey, value);
    EXPECT_EQ(-1, sRet);
}

// 测试值长度超出限制的情况
TEST_F(Util_SharedBinaryTree, TestValueLengthLimit) {
    std::string key = "value_len_limit_key";
    std::string longValue(SHARED_BTREE_VALUE_MAX_LEN, 'a');

    int sRet = mpTree->SetValue(key, longValue);
    EXPECT_EQ(-1, sRet);
}

// 测试更新已存在键的值
TEST_F(Util_SharedBinaryTree, TestUpdateValue) {
    std::string key = "update_key";
    std::string value1 = "value1";
    std::string value2 = "value2";

    int sRet1 = mpTree->SetValue(key, value1);
    EXPECT_EQ(0, sRet1);

    int sRet2 = mpTree->SetValue(key, value2);
    EXPECT_EQ(0, sRet2);

    std::string actualValue;
    int gRet = mpTree->GetValue(key, actualValue);
    EXPECT_EQ(0, gRet);
    EXPECT_EQ(value2, actualValue);
}

// 测试获取不存在的键的值
TEST_F(Util_SharedBinaryTree, TestGetNonExistentKey) {
    std::string nonExistKey = "non_exist_key";
    std::string value;

    int gRet = mpTree->GetValue(nonExistKey, value);
    EXPECT_EQ(-1, gRet);
}

// 测试共享内存空间不足的情况
TEST_F(Util_SharedBinaryTree, TestSharedMemoryFull) {
    SharedBinaryTree smallTree("/tmp/small_shared_tree", sizeof(size_t) + sizeof(Node));
    std::string key1 = "mem_full_key";
    std::string value1 = "mem_full_value";
    std::string actualValue1;

    int sRet1 = smallTree.SetValue(key1, value1);
    int gRet1 = smallTree.GetValue(key1, actualValue1);
    EXPECT_EQ(0, sRet1);
    EXPECT_EQ(0, gRet1);
    EXPECT_EQ(value1, actualValue1);

    std::string key2 = "mem_full_key2";
    std::string value2 = "mem_full_value2";
    int sRet2 = smallTree.SetValue(key2, value2);
    int gRet2 = smallTree.GetValue(key2, actualValue1);
    EXPECT_EQ(-1, sRet2);
    EXPECT_EQ(-1, gRet2);
}

// 测试关闭后重新打开共享内存二叉树，验证数据复用能力
TEST_F(Util_SharedBinaryTree, TestReopenAndReuse) {
    // 首次写入数据
    std::string key = "reuse_key";
    std::string value = "reuse_value";
    EXPECT_EQ(0, mpTree->SetValue(key, value));

    // 销毁当前实例
    mpTree = nullptr;

    // 重新打开共享内存
    mpTree = std::make_shared<SharedBinaryTree>("/tmp/test_shared_tree", SHARED_BTREE_MAX_SIZE, false);
    ASSERT_TRUE(nullptr != mpTree);

    // 验证原有数据可复用
    std::string actualValue;
    EXPECT_EQ(0, mpTree->GetValue(key, actualValue));
    EXPECT_EQ(value, actualValue);

    // 验证可在复用基础上新增数据
    std::string newKey = "new_reuse_key";
    std::string newValue = "new_reuse_value";
    EXPECT_EQ(0, mpTree->SetValue(newKey, newValue));
}

// 测试多个实例同时复用同一共享内存二叉树
TEST_F(Util_SharedBinaryTree, TestMultipleInstancesReuse) {
    // 实例1写入基础数据
    std::string key = "multi_reuse_key";
    std::string value = "initial_value";
    EXPECT_EQ(0, mpTree->SetValue(key, value));

    // 创建第二个实例复用同一共享内存
    std::shared_ptr<SharedBinaryTree> pTree2 = std::make_shared<SharedBinaryTree>("/tmp/test_shared_tree", SHARED_BTREE_MAX_SIZE, false);
    ASSERT_TRUE(nullptr != pTree2);

    // 实例2读取并更新数据
    std::string actualValue;
    EXPECT_EQ(0, pTree2->GetValue(key, actualValue));
    EXPECT_EQ(value, actualValue);

    std::string updateValue = "updated_by_tree2";
    EXPECT_EQ(0, pTree2->SetValue(key, updateValue));

    // 实例1验证更新后的数据
    EXPECT_EQ(0, mpTree->GetValue(key, actualValue));
    EXPECT_EQ(updateValue, actualValue);
}

// 测试多次复用二叉树进行连续操作
TEST_F(Util_SharedBinaryTree, TestContinuousReuseOperations) {
    const int OPER_COUNT = 3;
    std::string baseKey = "reuse_op_";

    // 第一次操作：写入初始数据
    for (int i = 0; i < OPER_COUNT; ++i) {
        std::string key = baseKey + std::to_string(i);
        std::string value = "init_" + std::to_string(i);
        EXPECT_EQ(0, mpTree->SetValue(key, value));
    }

    // 销毁并重新打开（第一次复用）
    mpTree = std::make_shared<SharedBinaryTree>("/tmp/test_shared_tree", SHARED_BTREE_MAX_SIZE, false);
    ASSERT_TRUE(nullptr != mpTree);

    // 第二次操作：更新数据
    for (int i = 0; i < OPER_COUNT; ++i) {
        std::string key = baseKey + std::to_string(i);
        std::string value = "update1_" + std::to_string(i);
        EXPECT_EQ(0, mpTree->SetValue(key, value));
    }

    // 销毁并重新打开（第二次复用）
    mpTree = std::make_shared<SharedBinaryTree>("/tmp/test_shared_tree", SHARED_BTREE_MAX_SIZE, false);
    ASSERT_TRUE(nullptr != mpTree);

    // 第三次操作：验证最终数据
    for (int i = 0; i < OPER_COUNT; ++i) {
        std::string key = baseKey + std::to_string(i);
        std::string expectedValue = "update1_" + std::to_string(i);
        std::string actualValue;

        EXPECT_EQ(0, mpTree->GetValue(key, actualValue));
        EXPECT_EQ(expectedValue, actualValue);
    }
}

// 测试128KB大小的共享内存二叉树能存储的最大key数量
TEST_F(Util_SharedBinaryTree, TestMaxKeyCountIn128KB) {
    std::shared_ptr<SharedBinaryTree> pTree128KB = std::make_shared<SharedBinaryTree>("/tmp/test_128kb_shared_tree", 128 * 1024, true);
    ASSERT_TRUE(nullptr != pTree128KB) << "Create 128KB failed!";
    int i = 0;
    for (; i < 100000; ++i) {
        std::string key = "k" + std::to_string(i);
        int ret = pTree128KB->SetValue(key, key + std::to_string(i));
        if (ret == -1) {
            break;
        }
    }

    TEST_LOG("Max insert %d keys into 128KB", i);
    unlink("/tmp/test_128kb_shared_tree");
}
