/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestCAtom.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : CAtom 编解码框架原子节点内部测试
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
#include <string>
#include <vector>
#include "CAtom.h"
#include "CDefine.h"
#include "gtest/gtest.h"

// 测试用派生类，暴露 protected 的 ResetDePos（Decode 前需重置位置）
class TestAtom : public CAtom {
public:
    explicit TestAtom() : CAtom(nullptr) {}
    void ResetPos() { ResetDePos(0); }
};

// 测试通过父节点构造
TEST(UtilModules_CAtom, ConstructWithParent)
{
    CAtom atom(nullptr);
    EXPECT_FALSE(atom.IsField());
}

// 测试通过父节点、名称、值构造
TEST(UtilModules_CAtom, ConstructWithNameAndValue)
{
    std::vector<uint8_t> value = {0x01, 0x02};
    CAtom atom(nullptr, "field1", value);

    std::vector<uint8_t> out;
    EXPECT_EQ(atom.GetValue(out), 0);
    EXPECT_EQ(out, value);
    EXPECT_EQ(atom.GetName(), "field1");
}

// 测试设置与获取值
TEST(UtilModules_CAtom, SetAndGetValue)
{
    CAtom atom(nullptr);

    std::vector<uint8_t> value = {0xAA, 0xBB, 0xCC};
    EXPECT_EQ(atom.SetValue(value), 0);

    std::vector<uint8_t> out;
    EXPECT_EQ(atom.GetValue(out), 0);
    EXPECT_EQ(out, value);
}

// 测试带名称的设置/获取值，名称不匹配返回失败
TEST(UtilModules_CAtom, SetGetValueWithName)
{
    CAtom atom(nullptr, "myfield", {});

    std::vector<uint8_t> value = {0x11};
    EXPECT_EQ(atom.SetValue("myfield", value), 0);
    EXPECT_EQ(atom.SetValue("other", value), -1);

    std::vector<uint8_t> out;
    EXPECT_EQ(atom.GetValue("myfield", out), 0);
    EXPECT_EQ(out, value);
    EXPECT_EQ(atom.GetValue("other", out), -1);
}

// 测试克隆返回独立对象
TEST(UtilModules_CAtom, Clone)
{
    std::vector<uint8_t> value = {0x01, 0x02, 0x03};
    CAtom atom(nullptr, "src", value);

    auto clone = atom.Clone();
    ASSERT_TRUE(clone != nullptr);

    std::vector<uint8_t> out;
    auto clonedAtom = std::dynamic_pointer_cast<CAtom>(clone);
    ASSERT_TRUE(clonedAtom != nullptr);
    EXPECT_EQ(clonedAtom->GetValue(out), 0);
    EXPECT_EQ(out, value);
}

// 测试按路径查找节点返回空
TEST(UtilModules_CAtom, GetNodeByPathReturnsNull)
{
    CAtom atom(nullptr);
    EXPECT_EQ(atom.GetNodeByPath("some/path"), nullptr);
}

// 测试编码追加值
TEST(UtilModules_CAtom, Encode)
{
    std::vector<uint8_t> value = {0xDE, 0xAD};
    CAtom atom(nullptr, "enc", value);

    std::vector<uint8_t> bytes = {0x01};
    EXPECT_EQ(atom.Encode(bytes), 2);
    EXPECT_EQ(bytes.size(), 3u);
    EXPECT_EQ(bytes[0], 0x01);
    EXPECT_EQ(bytes[1], 0xDE);
    EXPECT_EQ(bytes[2], 0xAD);
}

// 测试十六进制转储
TEST(UtilModules_CAtom, DumpHexValue)
{
    std::vector<uint8_t> value = {0x01, 0xFF};
    CAtom atom(nullptr, "dump", value);

    std::string hex = atom.DumpHexValue();
    EXPECT_FALSE(hex.empty());
}

// 测试解码 u8 类型
TEST(UtilModules_CAtom, DecodeU8)
{
    TestAtom atom;
    atom.SetName("field");
    atom.SetType(TEXT_TYPE_U8);
    atom.ResetPos();

    std::vector<uint8_t> bytes = {0x12, 0x34};
    EXPECT_EQ(atom.Decode(bytes), 1);

    std::vector<uint8_t> out;
    EXPECT_EQ(atom.GetValue(out), 0);
    EXPECT_EQ(out.size(), 1u);
    EXPECT_EQ(out[0], 0x12);
}

// 测试解码无效类型返回失败
TEST(UtilModules_CAtom, DecodeInvalidType)
{
    TestAtom atom;
    atom.SetName("field");
    atom.SetType("invalid_type");
    atom.ResetPos();

    std::vector<uint8_t> bytes = {0x01};
    EXPECT_LT(atom.Decode(bytes), 0);
}

// 测试解码空数据返回失败（位置越界）
TEST(UtilModules_CAtom, DecodeEmptyDataFails)
{
    TestAtom atom;
    atom.SetName("field");
    atom.SetType(TEXT_TYPE_U8);
    atom.ResetPos();

    std::vector<uint8_t> empty;
    EXPECT_EQ(atom.Decode(empty), -1);
}

// 测试解码 u32 类型
TEST(UtilModules_CAtom, DecodeU32)
{
    TestAtom atom;
    atom.SetName("field");
    atom.SetType(TEXT_TYPE_U32);
    atom.ResetPos();

    std::vector<uint8_t> bytes = {0x01, 0x02, 0x03, 0x04};
    EXPECT_EQ(atom.Decode(bytes), 4);

    std::vector<uint8_t> out;
    EXPECT_EQ(atom.GetValue(out), 0);
    ASSERT_EQ(out.size(), 4u);
    EXPECT_EQ(out[0], 0x01);
    EXPECT_EQ(out[3], 0x04);
}

// 测试解码数据不足返回长度（不更新值）
TEST(UtilModules_CAtom, DecodeInsufficientDataFails)
{
    TestAtom atom;
    atom.SetName("field");
    atom.SetType(TEXT_TYPE_U32);
    atom.ResetPos();

    std::vector<uint8_t> bytes = {0x01, 0x02};   // 不足 4 字节
    // 数据不足时 Decode 返回 len（4），但 mValue 不更新
    EXPECT_EQ(atom.Decode(bytes), 4);

    std::vector<uint8_t> out;
    EXPECT_EQ(atom.GetValue(out), 0);
    EXPECT_TRUE(out.empty());
}
