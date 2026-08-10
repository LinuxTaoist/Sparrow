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
 */
#include <string>
#include <vector>
#include <stdint.h>
#include "SprMsg.h"
#include "gtest/gtest.h"

namespace {
struct TestData {
    int32_t id;
    double  value;
    char    name[16];
};
} // anonymous namespace

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

// 测试构造函数：缺省
TEST_F(Core_SprMsg, DefaultConstructorResetsAllFields) {
    SprMsg msg;
    EXPECT_EQ(0u, msg.GetFrom());
    EXPECT_EQ(0u, msg.GetTo());
    EXPECT_EQ(0u, msg.GetMsgId());
    EXPECT_EQ(0, msg.GetSize());
    EXPECT_EQ(false, msg.GetBoolValue());
    EXPECT_EQ((uint8_t)0xFF, msg.GetU8Value());
    EXPECT_EQ((int8_t)0x7F, msg.GetI8Value());
}

// 测试构造函数：仅 msgId
TEST_F(Core_SprMsg, MsgIdOnly) {
    SprMsg msg(0xABCD);
    EXPECT_EQ(0u, msg.GetFrom());
    EXPECT_EQ(0u, msg.GetTo());
    EXPECT_EQ(0xABCDu, msg.GetMsgId());
}

// 测试构造函数：(to, msgId)
TEST_F(Core_SprMsg, ToAndMsgId) {
    SprMsg msg(100, 0x5678);
    EXPECT_EQ(0u, msg.GetFrom());
    EXPECT_EQ(100u, msg.GetTo());
    EXPECT_EQ(0x5678u, msg.GetMsgId());
}

// 测试构造函数：(to, srcMsg)
TEST_F(Core_SprMsg, ToAndSrcMsgCopiesData) {
    SprMsg src(0x2222);
    src.SetFrom(10);
    src.SetString("source data");

    SprMsg msg(50, src);
    EXPECT_EQ(10u, msg.GetFrom());  // src's From is preserved
    EXPECT_EQ(50u, msg.GetTo());
    EXPECT_EQ(0x2222u, msg.GetMsgId());
    EXPECT_EQ(src.GetString(), msg.GetString());
}

// 测试构造函数：(from, to, msgId)
TEST_F(Core_SprMsg, FromToMsgId) {
    SprMsg msg(10, 20, 0x3333);
    EXPECT_EQ(10u, msg.GetFrom());
    EXPECT_EQ(20u, msg.GetTo());
    EXPECT_EQ(0x3333u, msg.GetMsgId());
}

// 测试构造函数：拷贝构造
TEST_F(Core_SprMsg, CopyConstructorDeepCopiesAllFields) {
    SprMsg src(11, 22, 0xCAFE);
    src.SetU32Value(0x12345678);
    src.SetString("copy test");
    src.SetU8Value(0xAB);

    SprMsg dst(src);
    EXPECT_EQ(src.GetFrom(), dst.GetFrom());
    EXPECT_EQ(src.GetTo(), dst.GetTo());
    EXPECT_EQ(src.GetMsgId(), dst.GetMsgId());
    EXPECT_EQ(src.GetU32Value(), dst.GetU32Value());
    EXPECT_EQ(src.GetString(), dst.GetString());
    EXPECT_EQ(src.GetU8Value(), dst.GetU8Value());
}

// 测试构造函数：从编码字符串解码
TEST_F(Core_SprMsg, FromEncodedString) {
    SprMsg src(7, 8, 0xBEEF);
    src.SetU32Value(0xDEADBEEF);
    src.SetString("decoded");

    std::string encoded;
    src.Encode(encoded);

    SprMsg msg(encoded);
    EXPECT_EQ(src.GetFrom(), msg.GetFrom());
    EXPECT_EQ(src.GetTo(), msg.GetTo());
    EXPECT_EQ(src.GetMsgId(), msg.GetMsgId());
    EXPECT_EQ(src.GetU32Value(), msg.GetU32Value());
    EXPECT_EQ(src.GetString(), msg.GetString());
}

// 测试Set/Get基础类型
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

// 测试Bool类型true/false
TEST_F(Core_SprMsg, BoolTrueAndFalse) {
    SprMsg msg;
    msg.SetBoolValue(true);
    std::string encTrue;
    msg.Encode(encTrue);

    SprMsg decoded;
    decoded.Decode(encTrue);
    EXPECT_TRUE(decoded.GetBoolValue());

    msg.Clear();
    msg.SetBoolValue(false);
    std::string encFalse;
    msg.Encode(encFalse);
    decoded.Decode(encFalse);
    EXPECT_FALSE(decoded.GetBoolValue());
}

// 测试Set/Get From/To/MsgId
TEST_F(Core_SprMsg, FromToSetAndGet) {
    SprMsg msg;
    msg.SetFrom(0xAAAAAAAA);
    msg.SetTo(0xBBBBBBBB);
    msg.SetMsgId(0xCCCCCCCC);
    EXPECT_EQ(0xAAAAAAAAu, msg.GetFrom());
    EXPECT_EQ(0xBBBBBBBBu, msg.GetTo());
    EXPECT_EQ(0xCCCCCCCCu, msg.GetMsgId());
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

// 测试空字符串
TEST_F(Core_SprMsg, EmptyString) {
    SprMsg msg(0x4444);
    msg.SetString("");
    std::string encoded;
    msg.Encode(encoded);

    SprMsg decoded;
    decoded.Decode(encoded);
    EXPECT_EQ("", decoded.GetString());
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

// 测试空Vec
TEST_F(Core_SprMsg, EmptyVec) {
    SprMsg msg(0x5555);
    std::vector<uint32_t> emptyVec;
    msg.SetU32Vec(emptyVec);
    std::string encoded;
    msg.Encode(encoded);

    SprMsg decoded;
    decoded.Decode(encoded);
    std::vector<uint32_t> out = decoded.GetU32Vec();
    EXPECT_TRUE(out.empty());
}

// 测试SetDatas/GetDatas模板往返
TEST_F(Core_SprMsg, SetDatasAndGetDatasRoundtrip) {
    SprMsg msg(0x9999);
    auto src = std::make_shared<TestData>();
    src->id = 42;
    src->value = 3.14159;
    snprintf(src->name, sizeof(src->name), "test-name");

    msg.SetDatas(src, sizeof(TestData));

    std::string encoded;
    msg.Encode(encoded);

    SprMsg decoded;
    decoded.Decode(encoded);
    auto out = decoded.GetDatas<TestData>();
    ASSERT_TRUE(out != nullptr);
    EXPECT_EQ(src->id, out->id);
    EXPECT_DOUBLE_EQ(src->value, out->value);
    EXPECT_STREQ(src->name, out->name);
}

// 测试GetDatas数据不足时返回nullptr
TEST_F(Core_SprMsg, GetDatasInsufficientDataReturnsNull) {
    SprMsg msg(0xAAAA);
    std::vector<uint8_t> tiny = {0x01, 0x02};
    auto src = std::make_shared<std::vector<uint8_t>>(tiny);
    msg.SetDatas(src, tiny.size());

    std::string encoded;
    msg.Encode(encoded);
    SprMsg decoded;
    decoded.Decode(encoded);

    auto out = decoded.GetDatas<TestData>();
    EXPECT_TRUE(out == nullptr);
}

// 测试全部标量类型组合
TEST_F(Core_SprMsg, AllScalarTypesInOneMessage) {
    SprMsg msg(0xAA, 0xBB, 0xCCCC);
    msg.SetBoolValue(true);
    msg.SetU8Value(0x01);
    msg.SetI8Value(-1);
    msg.SetU16Value(0xFFFF);
    msg.SetI16Value(-32768);
    msg.SetU32Value(0xDEADBEEF);
    msg.SetI32Value(-0x7FFFFFFF - 1);
    msg.SetU64Value(0xFFFFFFFFFFFFFFFF);
    msg.SetI64Value(0x7FFFFFFFFFFFFFFF);

    std::string encoded;
    msg.Encode(encoded);
    SprMsg decoded;
    decoded.Decode(encoded);

    EXPECT_EQ(msg.GetFrom(), decoded.GetFrom());
    EXPECT_EQ(msg.GetTo(), decoded.GetTo());
    EXPECT_EQ(msg.GetMsgId(), decoded.GetMsgId());
    EXPECT_EQ(msg.GetBoolValue(), decoded.GetBoolValue());
    EXPECT_EQ(msg.GetU8Value(), decoded.GetU8Value());
    EXPECT_EQ(msg.GetI8Value(), decoded.GetI8Value());
    EXPECT_EQ(msg.GetU16Value(), decoded.GetU16Value());
    EXPECT_EQ(msg.GetI16Value(), decoded.GetI16Value());
    EXPECT_EQ(msg.GetU32Value(), decoded.GetU32Value());
    EXPECT_EQ(msg.GetI32Value(), decoded.GetI32Value());
    EXPECT_EQ(msg.GetU64Value(), decoded.GetU64Value());
    EXPECT_EQ(msg.GetI64Value(), decoded.GetI64Value());
}

// 测试全部Vec类型组合
TEST_F(Core_SprMsg, AllVecTypesInOneMessage) {
    SprMsg msg(0xAA, 0xBB, 0xFFFF);
    msg.SetU8Vec({0x01, 0x02, 0x03});
    msg.SetI8Vec({-1, -2, -3});
    msg.SetU16Vec({0x1234, 0x5678});
    msg.SetI16Vec({-0x1234, -0x5678});
    msg.SetU32Vec({0xDEADBEEF, 0xCAFEBABE});
    msg.SetI32Vec({-1000000, -2000000});
    msg.SetU64Vec({0x0123456789ABCDEF, 0xFEDCBA9876543210});
    msg.SetI64Vec({-0x0123456789ABCDEF, -0xFEDCBA9876543210});

    std::string encoded;
    msg.Encode(encoded);
    SprMsg decoded;
    decoded.Decode(encoded);

    std::vector<uint8_t>  u8v  = decoded.GetU8Vec();
    std::vector<int8_t>   i8v  = decoded.GetI8Vec();
    std::vector<uint16_t> u16v = decoded.GetU16Vec();
    std::vector<int16_t>  i16v = decoded.GetI16Vec();
    std::vector<uint32_t> u32v = decoded.GetU32Vec();
    std::vector<int32_t>  i32v = decoded.GetI32Vec();
    std::vector<uint64_t> u64v = decoded.GetU64Vec();
    std::vector<int64_t>  i64v = decoded.GetI64Vec();

    ASSERT_EQ(msg.GetU8Vec().size(),  u8v.size());
    ASSERT_EQ(msg.GetI8Vec().size(),  i8v.size());
    ASSERT_EQ(msg.GetU16Vec().size(), u16v.size());
    ASSERT_EQ(msg.GetI16Vec().size(), i16v.size());
    ASSERT_EQ(msg.GetU32Vec().size(), u32v.size());
    ASSERT_EQ(msg.GetI32Vec().size(), i32v.size());
    ASSERT_EQ(msg.GetU64Vec().size(), u64v.size());
    ASSERT_EQ(msg.GetI64Vec().size(), i64v.size());

    for (size_t i = 0; i < u8v.size();  ++i) EXPECT_EQ(msg.GetU8Vec()[i],  u8v[i]);
    for (size_t i = 0; i < i8v.size();  ++i) EXPECT_EQ(msg.GetI8Vec()[i],  i8v[i]);
    for (size_t i = 0; i < u16v.size(); ++i) EXPECT_EQ(msg.GetU16Vec()[i], u16v[i]);
    for (size_t i = 0; i < i16v.size(); ++i) EXPECT_EQ(msg.GetI16Vec()[i], i16v[i]);
    for (size_t i = 0; i < u32v.size(); ++i) EXPECT_EQ(msg.GetU32Vec()[i], u32v[i]);
    for (size_t i = 0; i < i32v.size(); ++i) EXPECT_EQ(msg.GetI32Vec()[i], i32v[i]);
    for (size_t i = 0; i < u64v.size(); ++i) EXPECT_EQ(msg.GetU64Vec()[i], u64v[i]);
    for (size_t i = 0; i < i64v.size(); ++i) EXPECT_EQ(msg.GetI64Vec()[i], i64v[i]);
}

// 测试拷贝赋值
TEST_F(Core_SprMsg, AssignmentOperator) {
    SprMsg src(1, 2, 0x1111);
    src.SetI32Value(-12345);
    src.SetString("assign");

    SprMsg dst;
    dst = src;
    EXPECT_EQ(src.GetFrom(), dst.GetFrom());
    EXPECT_EQ(src.GetTo(), dst.GetTo());
    EXPECT_EQ(src.GetMsgId(), dst.GetMsgId());
    EXPECT_EQ(src.GetI32Value(), dst.GetI32Value());
    EXPECT_EQ(src.GetString(), dst.GetString());
}

// 测试CopyMsg拷贝所有类型
TEST_F(Core_SprMsg, CopyMsgAllTypes) {
    SprMsg src;
    src.SetFrom(0xA0);
    src.SetTo(0xB0);
    src.SetMsgId(0xC0C0);
    src.SetBoolValue(true);
    src.SetU8Value(0x42);
    src.SetI8Value(-8);
    src.SetU16Value(0xBEEF);
    src.SetI16Value(-0x7FF0);
    src.SetU32Value(0xCAFEBABE);
    src.SetI32Value(-999999);
    src.SetU64Value(0x0123456789ABCDEF);
    src.SetI64Value(-0x0FEDCBA987654321);
    src.SetString("CopyMsg all types");

    SprMsg dst;
    dst.CopyMsg(src);
    EXPECT_EQ(src.GetFrom(), dst.GetFrom());
    EXPECT_EQ(src.GetTo(), dst.GetTo());
    EXPECT_EQ(src.GetMsgId(), dst.GetMsgId());
    EXPECT_EQ(src.GetBoolValue(), dst.GetBoolValue());
    EXPECT_EQ(src.GetU8Value(), dst.GetU8Value());
    EXPECT_EQ(src.GetI8Value(), dst.GetI8Value());
    EXPECT_EQ(src.GetU16Value(), dst.GetU16Value());
    EXPECT_EQ(src.GetI16Value(), dst.GetI16Value());
    EXPECT_EQ(src.GetU32Value(), dst.GetU32Value());
    EXPECT_EQ(src.GetI32Value(), dst.GetI32Value());
    EXPECT_EQ(src.GetU64Value(), dst.GetU64Value());
    EXPECT_EQ(src.GetI64Value(), dst.GetI64Value());
    EXPECT_EQ(src.GetString(), dst.GetString());
}

// 测试Clear重置所有字段
TEST_F(Core_SprMsg, ClearResetsAllFields) {
    SprMsg msg;
    msg.SetFrom(5);
    msg.SetTo(6);
    msg.SetMsgId(7);
    msg.SetBoolValue(true);
    msg.SetString("to be cleared");
    msg.SetU32Value(0xFFFFFFFF);

    msg.Clear();
    EXPECT_EQ(0u, msg.GetFrom());
    EXPECT_EQ(0u, msg.GetTo());
    EXPECT_EQ(0u, msg.GetMsgId());
    EXPECT_EQ("", msg.GetString());
    EXPECT_EQ(false, msg.GetBoolValue());
    EXPECT_EQ((uint8_t)0xFF, msg.GetU8Value());
    EXPECT_EQ((uint32_t)0xFFFFFFFF, msg.GetU32Value());
}

// 测试SetSize/GetSize
TEST_F(Core_SprMsg, SetAndGetSize) {
    SprMsg msg;
    msg.SetSize(0x12345);
    EXPECT_EQ(0x12345, msg.GetSize());

    msg.SetString("test");
    std::string encoded;
    msg.Encode(encoded);
    SprMsg decoded;
    int32_t encSize = (int32_t)encoded.size();
    decoded.Decode(encoded);
    EXPECT_EQ(encSize, decoded.GetSize());
}

// 测试DumpBrief关键字段
TEST_F(Core_SprMsg, DumpBriefContainsKeyFields) {
    SprMsg msg(10, 20, 0x3333);
    msg.SetString("hello");
    msg.SetU32Value(42);

    std::string brief = msg.DumpBrief();
    EXPECT_NE(std::string::npos, brief.find("From:10"));
    EXPECT_NE(std::string::npos, brief.find("To:20"));
    EXPECT_NE(std::string::npos, brief.find("MsgId:13107"));
    EXPECT_NE(std::string::npos, brief.find("\"String\":\"hello\""));
    EXPECT_NE(std::string::npos, brief.find("\"U32Value\":42"));
}

// 测试DumpBrief空消息
TEST_F(Core_SprMsg, DumpBriefEmptyMessage) {
    SprMsg msg(0, 0, 0);
    std::string brief = msg.DumpBrief();
    EXPECT_NE(std::string::npos, brief.find("From:0"));
    EXPECT_NE(std::string::npos, brief.find("Data: {}"));
}

// 测试DumpDetails JSON格式
TEST_F(Core_SprMsg, DumpDetailsJsonFormat) {
    SprMsg msg(1, 2, 3);
    msg.SetBoolValue(true);
    msg.SetString("json");

    std::string details = msg.DumpDetails();
    EXPECT_NE(std::string::npos, details.find("\"From\""));
    EXPECT_NE(std::string::npos, details.find("\"To\""));
    EXPECT_NE(std::string::npos, details.find("\"MsgId\""));
    EXPECT_NE(std::string::npos, details.find("\"Data\""));
    EXPECT_NE(std::string::npos, details.find("\"BoolValue\":true"));
    EXPECT_NE(std::string::npos, details.find("\"String\":\"json\""));
}

// 测试解码损坏数据失败后清空旧数据
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

// 测试解码残缺字符串失败
TEST_F(Core_SprMsg, DecodeInvalidStringFailTest) {
    expectMsg.SetString("Hello, Test String!");

    std::string encoded;
    expectMsg.Encode(encoded);
    encoded.pop_back();

    EXPECT_EQ(-1, actualMsg.Decode(encoded));
    EXPECT_EQ("", actualMsg.GetString());
}

// 测试空消息编解码
TEST_F(Core_SprMsg, EmptyMessageEncodeDecode) {
    SprMsg msg;
    std::string encoded;
    msg.Encode(encoded);
    EXPECT_GT(encoded.size(), 0u);

    SprMsg decoded;
    decoded.Decode(encoded);
    EXPECT_EQ(msg.GetFrom(), decoded.GetFrom());
    EXPECT_EQ(msg.GetTo(), decoded.GetTo());
    EXPECT_EQ(msg.GetMsgId(), decoded.GetMsgId());
}

// 测试解码残缺帧头
TEST_F(Core_SprMsg, DecodeTruncatedFrameHeader) {
    SprMsg msg;
    std::string truncated1;
    EXPECT_EQ(-1, msg.Decode(truncated1));

    std::string truncated2(4, 'A');
    EXPECT_EQ(-1, msg.Decode(truncated2));
}

// 测试解码垃圾数据
TEST_F(Core_SprMsg, DecodeGarbageData) {
    SprMsg msg;
    std::string garbage(64, '\xFF');
    int32_t ret = msg.Decode(garbage);
    EXPECT_NE(0, ret);
}
