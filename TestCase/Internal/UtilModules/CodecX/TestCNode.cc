/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestCNode.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : CNode 编解码框架节点基类内部测试
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
#include "CNode.h"
#include "CAtom.h"
#include "CDefine.h"
#include "gtest/gtest.h"

// 测试用派生类，暴露 CNode 的 protected 方法
class TestNode : public CAtom {
public:
    explicit TestNode() : CAtom(nullptr) {}

    void SetSIdx(int32_t v)     { SetSIndex(v); }
    int32_t GetSIdx()           { return GetSIndex(); }
    void SetEIdx(int32_t v)     { SetEIndex(v); }
    int32_t GetEIdx()           { return GetEIndex(); }
    void SetLen(int32_t v)      { SetLength(v); }
    int32_t GetLen()            { return GetLength(); }
    void SetEnd(CEndianType v)  { SetEndian(v); }
    int32_t GetEnd()            { return GetEndian(); }
    void ResetEPos(int32_t v)   { ResetEnPos(v); }
    void EPosAdd(int32_t v)     { EnPosAdd(v); }
    int32_t GetEPos()           { return GetEnPos(); }
    void DPosAdd(int32_t v)     { DePosAdd(v); }
    int32_t GetDPos()           { return GetDePos(); }
    void ResetDPos(int32_t v)   { ResetDePos(v); }
};

// 测试构造默认值
TEST(UtilModules_CNode, DefaultConstruction)
{
    TestNode node;
    EXPECT_FALSE(node.IsField());
    EXPECT_EQ(node.GetName(), "");
    EXPECT_EQ(node.GetType(), "");
    EXPECT_EQ(node.GetSIdx(), -1);
    EXPECT_EQ(node.GetEIdx(), -1);
    EXPECT_EQ(node.GetLen(), -1);
    EXPECT_EQ(node.GetEnd(), CENDIAN_BIG);
}

// 测试名称与类型设置
TEST(UtilModules_CNode, NameAndType)
{
    TestNode node;
    node.SetName("node1");
    node.SetType("u32");
    EXPECT_EQ(node.GetName(), "node1");
    EXPECT_EQ(node.GetType(), "u32");
}

// 测试索引设置与获取
TEST(UtilModules_CNode, IndexSetGet)
{
    TestNode node;
    node.SetSIdx(10);
    node.SetEIdx(20);
    EXPECT_EQ(node.GetSIdx(), 10);
    EXPECT_EQ(node.GetEIdx(), 20);
}

// 测试长度设置与获取
TEST(UtilModules_CNode, LengthSetGet)
{
    TestNode node;
    node.SetLen(64);
    EXPECT_EQ(node.GetLen(), 64);
}

// 测试大小端设置与获取
TEST(UtilModules_CNode, EndianSetGet)
{
    TestNode node;
    node.SetEnd(CENDIAN_LITTLE);
    EXPECT_EQ(node.GetEnd(), CENDIAN_LITTLE);
    node.SetEnd(CENDIAN_BIG);
    EXPECT_EQ(node.GetEnd(), CENDIAN_BIG);
}

// 测试开关值设置与获取
TEST(UtilModules_CNode, SwitchValue)
{
    TestNode node;
    EXPECT_FALSE(node.HasSwitchValue());
    EXPECT_EQ(node.GetSwitchValue(), 0);

    node.SetSwitchValue(12345);
    EXPECT_TRUE(node.HasSwitchValue());
    EXPECT_EQ(node.GetSwitchValue(), 12345);
}

// 测试编码位置相关方法
TEST(UtilModules_CNode, EncodePosition)
{
    TestNode node;
    node.ResetEPos(5);
    EXPECT_EQ(node.GetEPos(), 5);
    node.EPosAdd(3);
    EXPECT_EQ(node.GetEPos(), 8);
}

// 测试解码位置相关方法
TEST(UtilModules_CNode, DecodePosition)
{
    TestNode node;
    node.ResetDPos(4);
    EXPECT_EQ(node.GetDPos(), 4);
    node.DPosAdd(2);
    EXPECT_EQ(node.GetDPos(), 6);
}

// 测试父子节点关系
TEST(UtilModules_CNode, ParentChildRelationship)
{
    auto parent = std::make_shared<TestNode>();
    auto child = std::make_shared<TestNode>();
    child->SetParentNode(parent);

    EXPECT_EQ(child->GetParentNode(), parent);
    EXPECT_EQ(child->GetRootNode(), parent);
    // 无父节点的根节点，GetRootNode 返回空
    EXPECT_EQ(parent->GetRootNode(), nullptr);
}

// 测试字符串值设置与获取
TEST(UtilModules_CNode, StrValueSetGet)
{
    TestNode node;
    EXPECT_EQ(node.SetStrValue("", "hello"), 0);

    std::string out;
    // GetStrValue 返回转换的字节数（5）
    EXPECT_EQ(node.GetStrValue("", out), 5);
    EXPECT_EQ(out, "hello");
}

// 测试拷贝构造
TEST(UtilModules_CNode, CopyConstructor)
{
    TestNode src;
    src.SetName("copied");
    src.SetType("u8");

    TestNode dst(src);
    EXPECT_EQ(dst.GetName(), "copied");
    EXPECT_EQ(dst.GetType(), "u8");
}

// 测试赋值运算符
TEST(UtilModules_CNode, AssignmentOperator)
{
    TestNode src;
    src.SetName("assigned");
    src.SetType("u16");

    TestNode dst;
    dst = src;
    EXPECT_EQ(dst.GetName(), "assigned");
    EXPECT_EQ(dst.GetType(), "u16");
}
