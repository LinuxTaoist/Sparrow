/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprMsg.cc
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
#include <string>
#include <vector>
#include <stdint.h>
#include "SprMsg.h"
#include "gtest/gtest.h"

class Core_SprMsg : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化公共测试数据
        expectMsg.SetFrom(1001);
        expectMsg.SetTo(2001);
        expectMsg.SetMsgId(0x1234);
    }

    SprMsg expectMsg;   // 期望的SprMsg
    SprMsg actualMsg;   // 实际的SprMsg
};

// 测试基础类型
TEST_F(Core_SprMsg, BasicTypesTest) {
    expectMsg.SetBoolValue(true);
    expectMsg.SetU8Value(0xFF);
    expectMsg.SetI8Value(-1);
    expectMsg.SetU16Value(0x1234);
    expectMsg.SetI16Value(-0x1234);
    expectMsg.SetU32Value(0x12345678);
    expectMsg.SetI32Value(-0x12345678);
    expectMsg.SetU64Value(0x123456789ABCDEF0);
    expectMsg.SetI64Value(-0x123456789ABCDEF0);

    // 编码解码
    std::string encoded;
    expectMsg.Encode(encoded);
    actualMsg.Decode(encoded);

    // 基础字段对比
    EXPECT_EQ(expectMsg.GetFrom(), actualMsg.GetFrom());
    EXPECT_EQ(expectMsg.GetTo(), actualMsg.GetTo());
    EXPECT_EQ(expectMsg.GetMsgId(), actualMsg.GetMsgId());

    // 基础类型对比
    EXPECT_EQ(static_cast<bool>(expectMsg.GetBoolValue()),
                static_cast<bool>(actualMsg.GetBoolValue()));
    EXPECT_EQ(static_cast<uint8_t>(expectMsg.GetU8Value()),
                static_cast<uint8_t>(actualMsg.GetU8Value()));
    EXPECT_EQ(static_cast<int8_t>(expectMsg.GetI8Value()),
                static_cast<int8_t>(actualMsg.GetI8Value()));
    EXPECT_EQ(static_cast<uint16_t>(expectMsg.GetU16Value()),
                static_cast<uint16_t>(actualMsg.GetU16Value()));
    EXPECT_EQ(static_cast<int16_t>(expectMsg.GetI16Value()),
                static_cast<int16_t>(actualMsg.GetI16Value()));
    EXPECT_EQ(static_cast<uint32_t>(expectMsg.GetU32Value()),
                static_cast<uint32_t>(actualMsg.GetU32Value()));
    EXPECT_EQ(static_cast<int32_t>(expectMsg.GetI32Value()),
                static_cast<int32_t>(actualMsg.GetI32Value()));
    EXPECT_EQ(static_cast<uint64_t>(expectMsg.GetU64Value()),
                static_cast<uint64_t>(actualMsg.GetU64Value()));
    EXPECT_EQ(static_cast<int64_t>(expectMsg.GetI64Value()),
                static_cast<int64_t>(actualMsg.GetI64Value()));
}

// 测试字符串
TEST_F(Core_SprMsg, StringTest) {
    const std::string testStr = "Hello, Test String!";
    expectMsg.SetString(testStr);

    std::string encoded;
    expectMsg.Encode(encoded);
    actualMsg.Decode(encoded);

    EXPECT_EQ(expectMsg.GetString(), actualMsg.GetString());
}

// 测试U8Vec类型
TEST_F(Core_SprMsg, U8VecTest) {
    std::vector<uint8_t> testVec = {0x01, 0x02, 0x03, 0x04};
    size_t expectedSize = testVec.size();

    std::string encoded;
    expectMsg.SetU8Vec(testVec);
    expectMsg.Encode(encoded);
    actualMsg.Decode(encoded);

    std::vector<uint8_t> actualVec = actualMsg.GetU8Vec();
    size_t actualSize = actualVec.size();

    EXPECT_EQ(expectedSize, actualSize);
    for (size_t i = 0; i < expectedSize && i < actualSize; ++i) {
        EXPECT_EQ(testVec[i], actualVec[i]);
    }
}

// 测试I8Vec类型
TEST_F(Core_SprMsg, I8VecTest) {
    std::vector<int8_t> testVec = {-1, -2, -3, -4};
    size_t expectedSize = testVec.size();

    std::string encoded;
    expectMsg.SetI8Vec(testVec);
    expectMsg.Encode(encoded);
    actualMsg.Decode(encoded);

    std::vector<int8_t> actualVec = actualMsg.GetI8Vec();
    size_t actualSize = actualVec.size();

    EXPECT_EQ(expectedSize, actualSize);
    for (size_t i = 0; i < expectedSize && i < actualSize; ++i) {
        EXPECT_EQ(testVec[i], actualVec[i]);
    }
}

// 测试U16Vec类型
TEST_F(Core_SprMsg, U16VecTest) {
    std::vector<uint16_t> testVec = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
    size_t expectedSize = testVec.size();

    std::string encoded;
    expectMsg.SetU16Vec(testVec);
    expectMsg.Encode(encoded);
    actualMsg.Decode(encoded);

    std::vector<uint16_t> actualVec = actualMsg.GetU16Vec();
    size_t actualSize = actualVec.size();

    EXPECT_EQ(expectedSize, actualSize);
    for (size_t i = 0; i < expectedSize && i < actualSize; ++i) {
        EXPECT_EQ(testVec[i], actualVec[i]);
    }
}

// 测试I16Vec类型
TEST_F(Core_SprMsg, I16VecTest) {
    std::vector<int16_t> testVec = {-0x1234, -0x5678, -0x7ABC, -0x7EF0};
    size_t expectedSize = testVec.size();

    std::string encoded;
    expectMsg.SetI16Vec(testVec);
    expectMsg.Encode(encoded);
    actualMsg.Decode(encoded);

    std::vector<int16_t> actualVec = actualMsg.GetI16Vec();
    size_t actualSize = actualVec.size();

    EXPECT_EQ(expectedSize, actualSize);
    for (size_t i = 0; i < expectedSize && i < actualSize; ++i) {
        EXPECT_EQ(testVec[i], actualVec[i]);
    }
}

// 测试U32Vec类型
TEST_F(Core_SprMsg, U32VecTest) {
    std::vector<uint32_t> testVec = {0x12345678, 0x9ABCDEF0, 0x13579BDF, 0x2468ACE0};
    size_t expectedSize = testVec.size();

    std::string encoded;
    expectMsg.SetU32Vec(testVec);
    expectMsg.Encode(encoded);
    actualMsg.Decode(encoded);

    std::vector<uint32_t> actualVec = actualMsg.GetU32Vec();
    size_t actualSize = actualVec.size();

    EXPECT_EQ(expectedSize, actualSize);
    for (size_t i = 0; i < expectedSize && i < actualSize; ++i) {
        EXPECT_EQ(testVec[i], actualVec[i]);
    }
}

// 测试I32Vec类型
TEST_F(Core_SprMsg, I32VecTest) {
    std::vector<int32_t> testVec = {-0x12345678, -0x9ABCDEF0, -0x13579BDF, -0x2468ACE0};
    size_t expectedSize = testVec.size();

    std::string encoded;
    expectMsg.SetI32Vec(testVec);
    expectMsg.Encode(encoded);
    actualMsg.Decode(encoded);

    std::vector<int32_t> actualVec = actualMsg.GetI32Vec();
    size_t actualSize = actualVec.size();

    EXPECT_EQ(expectedSize, actualSize);
    for (size_t i = 0; i < expectedSize && i < actualSize; ++i) {
        EXPECT_EQ(testVec[i], actualVec[i]);
    }
}

// 测试U64Vec类型
TEST_F(Core_SprMsg, U64VecTest) {
    std::vector<uint64_t> testVec = {0x123456789ABCDEF0, 0x0FEDCBA987654321, 0x1122334455667788, 0x8877665544332211};
    size_t expectedSize = testVec.size();

    std::string encoded;
    expectMsg.SetU64Vec(testVec);
    expectMsg.Encode(encoded);
    actualMsg.Decode(encoded);

    std::vector<uint64_t> actualVec = actualMsg.GetU64Vec();
    size_t actualSize = actualVec.size();

    EXPECT_EQ(expectedSize, actualSize);
    for (size_t i = 0; i < expectedSize && i < actualSize; ++i) {
        EXPECT_EQ(testVec[i], actualVec[i]);
    }
}

// 测试I64Vec类型
TEST_F(Core_SprMsg, I64VecTest) {
    std::vector<int64_t> testVec = {-0x123456789ABCDEF0, -0x0FEDCBA987654321, -0x1122334455667788, -0x8877665544332211};
    size_t expectedSize = testVec.size();

    std::string encoded;
    expectMsg.SetI64Vec(testVec);
    expectMsg.Encode(encoded);
    actualMsg.Decode(encoded);

    std::vector<int64_t> actualVec = actualMsg.GetI64Vec();
    size_t actualSize = actualVec.size();

    EXPECT_EQ(expectedSize, actualSize);
    for (size_t i = 0; i < expectedSize && i < actualSize; ++i) {
        EXPECT_EQ(testVec[i], actualVec[i]);
    }
}

TEST_F(Core_SprMsg, DecodeInvalidStringFailTest) {
    expectMsg.SetString("Hello, Test String!");

    std::string encoded;
    expectMsg.Encode(encoded);
    encoded.pop_back();

    EXPECT_EQ(-1, actualMsg.Decode(encoded));
    EXPECT_EQ("", actualMsg.GetString());
}

TEST_F(Core_SprMsg, DecodeFailShouldClearOldStringTest) {
    expectMsg.SetString("Old Value");

    std::string encoded;
    expectMsg.Encode(encoded);
    EXPECT_EQ(0, actualMsg.Decode(encoded));
    EXPECT_EQ("Old Value", actualMsg.GetString());

    SprMsg invalidMsg;
    invalidMsg.SetFrom(1001);
    invalidMsg.SetTo(2001);
    invalidMsg.SetMsgId(0x1234);
    invalidMsg.SetString("New Value");

    std::string invalidEncoded;
    invalidMsg.Encode(invalidEncoded);
    invalidEncoded.pop_back();

    EXPECT_EQ(-1, actualMsg.Decode(invalidEncoded));
    EXPECT_EQ("", actualMsg.GetString());
}
