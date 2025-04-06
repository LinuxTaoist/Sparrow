/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestRunningTiming.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/03/16
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
#include "gtest/gtest.h"
#include "SharedBinaryTree.h"

class Util_SharedBinaryTree : public ::testing::Test {
protected:
    void SetUp() override {
        tree = new (std::nothrow) SharedBinaryTree("/tmp/test_shared_tree", 1024);
        ASSERT_EQ(true, (tree != nullptr));
    }

    void TearDown() override {
        delete tree;
    }

    SharedBinaryTree* tree;
};

// 测试 SetValue 和 GetValue 功能
TEST_F(Util_SharedBinaryTree, TestSetAndGetValue) {
    std::string expectKey = "test_key";
    std::string expectValue = "test_value";

    int sRet = tree->SetValue(expectKey, expectValue);
    EXPECT_EQ(0, sRet);

    std::string actualValue;
    int gRet = tree->GetValue(expectKey, actualValue);
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
        int ret = tree->SetValue(pair.first, pair.second);
        EXPECT_EQ(0, ret);
    }

    std::map<std::string, std::string> actualMap;
    tree->GetAllKeyValues(actualMap);
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

    int sRet = tree->SetValue(longKey, value);
    EXPECT_EQ(-1, sRet);
}

// 测试值长度超出限制的情况
TEST_F(Util_SharedBinaryTree, TestValueLengthLimit) {
    std::string key = "value_len_limit_key";
    std::string longValue(SHARED_BTREE_VALUE_MAX_LEN, 'a');

    int sRet = tree->SetValue(key, longValue);
    EXPECT_EQ(-1, sRet);
}

// 测试更新已存在键的值
TEST_F(Util_SharedBinaryTree, TestUpdateValue) {
    std::string key = "update_key";
    std::string value1 = "value1";
    std::string value2 = "value2";

    int sRet1 = tree->SetValue(key, value1);
    EXPECT_EQ(0, sRet1);

    int sRet2 = tree->SetValue(key, value2);
    EXPECT_EQ(0, sRet2);

    std::string actualValue;
    int gRet = tree->GetValue(key, actualValue);
    EXPECT_EQ(0, gRet);
    EXPECT_EQ(value2, actualValue);
}

// 测试获取不存在的键的值
TEST_F(Util_SharedBinaryTree, TestGetNonExistentKey) {
    std::string nonExistKey = "non_exist_key";
    std::string value;

    int gRet = tree->GetValue(nonExistKey, value);
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
