/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestCField.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : CField 编解码框架字段节点内部测试
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
#include "CField.h"
#include "CAtom.h"
#include "CDefine.h"
#include "gtest/gtest.h"

// 测试构造与 IsField 默认值
TEST(UtilModules_CField, Construct)
{
    CField field(nullptr);
    EXPECT_TRUE(field.IsField());   // CField 默认 isField=true
}

// 测试长度引用设置与获取
TEST(UtilModules_CField, LenReferenceSetGet)
{
    CField field(nullptr);
    field.SetLenReference("bms_count");
    EXPECT_EQ(field.GetLenReference(), "bms_count");
}

// 测试长度模式设置与获取
TEST(UtilModules_CField, LenModeSetGet)
{
    CField field(nullptr);
    field.SetLenMode(TEXT_LEN_MODE_BYTES);
    EXPECT_EQ(field.GetLenMode(), TEXT_LEN_MODE_BYTES);
}

// 测试长度公式设置与获取
TEST(UtilModules_CField, LenFormulaSetGet)
{
    CField field(nullptr);
    field.SetLenFormula("curpos");
    EXPECT_EQ(field.GetLenFormula(), "curpos");
}

// 测试子节点容器标签设置与获取
TEST(UtilModules_CField, ChildNodesTagSetGet)
{
    CField field(nullptr);
    field.SetChildNodesTag("children");
    EXPECT_EQ(field.GetChildNodesTag(), "children");
}

// 测试添加/删除/查找子节点
TEST(UtilModules_CField, AddDelGetNode)
{
    CField field(nullptr);

    auto atom1 = std::make_shared<CAtom>(nullptr, "atom1", std::vector<uint8_t>{});
    auto atom2 = std::make_shared<CAtom>(nullptr, "atom2", std::vector<uint8_t>{});

    EXPECT_EQ(field.AddNode(atom1), 0);
    EXPECT_EQ(field.AddNode(atom2), 0);
    EXPECT_EQ(field.GetChildNodes().size(), 2u);

    EXPECT_EQ(field.GetNode("atom1"), atom1);
    EXPECT_EQ(field.GetNode("nonexist"), nullptr);

    EXPECT_EQ(field.DelNode(atom1), 0);
    EXPECT_EQ(field.GetChildNodes().size(), 1u);

    // 删除不存在的节点返回 -1
    EXPECT_EQ(field.DelNode(atom1), -1);
}

// 测试通过 atom 子节点设置与获取字符串值
TEST(UtilModules_CField, StrValueAtomChild)
{
    CField field(nullptr);
    auto atom = std::make_shared<CAtom>(nullptr, "atom1", std::vector<uint8_t>{});
    atom->SetType(TEXT_TYPE_U8);
    field.AddNode(atom);

    EXPECT_EQ(field.SetStrValue("atom1", "A"), 0);

    std::string out;
    EXPECT_EQ(field.GetStrValue("atom1", out), 1);
    EXPECT_EQ(out, "A");
}

// 测试空名称设置值返回失败
TEST(UtilModules_CField, SetValueEmptyNameFails)
{
    CField field(nullptr);
    EXPECT_EQ(field.SetStrValue("", "x"), -1);
}

// 测试设置不存在的子节点返回失败
TEST(UtilModules_CField, SetValueNonexistChildFails)
{
    CField field(nullptr);
    EXPECT_EQ(field.SetStrValue("nonexist", "x"), -1);
}

// 测试克隆字段及其子节点
TEST(UtilModules_CField, Clone)
{
    CField field(nullptr);
    field.SetName("root");
    auto atom = std::make_shared<CAtom>(nullptr, "atom1", std::vector<uint8_t>{0x01});
    field.AddNode(atom);

    auto clone = field.Clone();
    ASSERT_TRUE(clone != nullptr);

    auto clonedField = std::dynamic_pointer_cast<CField>(clone);
    ASSERT_TRUE(clonedField != nullptr);
    EXPECT_EQ(clonedField->GetName(), "root");
    EXPECT_EQ(clonedField->GetChildNodes().size(), 1u);
}

// 测试按相对路径查找子节点
TEST(UtilModules_CField, GetNodeByRelativePath)
{
    CField field(nullptr);
    field.SetName("root");
    auto atom = std::make_shared<CAtom>(nullptr, "atom1", std::vector<uint8_t>{});
    field.AddNode(atom);

    EXPECT_EQ(field.GetNodeByPath("atom1"), atom);
    EXPECT_EQ(field.GetNodeByPath("nonexist"), nullptr);
}

// 测试按绝对路径查找子节点
TEST(UtilModules_CField, GetNodeByAbsolutePath)
{
    CField root(nullptr);
    root.SetName("root");
    auto atom = std::make_shared<CAtom>(nullptr, "atom1", std::vector<uint8_t>{});
    root.AddNode(atom);

    EXPECT_EQ(root.GetNodeByPath("/root/atom1"), atom);
}

// 测试空路径查找返回空
TEST(UtilModules_CField, GetNodeByEmptyPath)
{
    CField field(nullptr);
    EXPECT_EQ(field.GetNodeByPath(""), nullptr);
}
