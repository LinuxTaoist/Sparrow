/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestIniParser.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : IniParser unit tests.
 *  @date       : 2026/09/20
 *
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include "IniParser.h"
#include "gtest/gtest.h"

namespace {

const char* TEST_INI_PATH = "/tmp/sparrow_ini_parser_test.ini";

class IniParserTest : public ::testing::Test
{
protected:
    void TearDown() override
    {
        std::remove(TEST_INI_PATH);
    }

    void WriteConfig(const std::string& content)
    {
        std::ofstream file(TEST_INI_PATH);
        ASSERT_TRUE(file.is_open());
        file << content;
    }
};

} // namespace

// 测试加载配置文件并读取 section、key 和 value
TEST_F(IniParserTest, LoadValuesAndSections)
{
    WriteConfig(
        "# common settings\n"
        "[logging]\n"
        "level = debug\n"
        "output=file\n"
        "\n"
        "[output.NetworkSrv]\n"
        "file_path=/tmp/network\n");

    IniParser parser;
    EXPECT_EQ(parser.Load(TEST_INI_PATH), 0);
    EXPECT_EQ(parser.GetValue("logging", "level"), "debug");
    EXPECT_EQ(parser.GetValue("logging", "output"), "file");
    EXPECT_EQ(parser.GetValue("output.NetworkSrv", "file_path"), "/tmp/network");

    const IniParser::Sections sections = parser.GetSections();
    ASSERT_EQ(sections.size(), 2U);
    EXPECT_EQ(sections.begin()->first, "logging");
    EXPECT_EQ(std::next(sections.begin())->first, "output.NetworkSrv");
    EXPECT_EQ(parser.GetSection("logging").at("level"), "debug");
}

// 测试忽略空行以及 #、; 开头的注释行
TEST_F(IniParserTest, IgnoreBlankLinesAndComments)
{
    WriteConfig(
        "; comment\n"
        "\n"
        "[section]\n"
        "# comment\n"
        "key=value # inline comment\n");

    IniParser parser;
    ASSERT_EQ(parser.Load(TEST_INI_PATH), 0);
    EXPECT_EQ(parser.GetValue("section", "key"), "value");
}

// 测试 section、key 和 value 两侧空白字符的裁剪
TEST_F(IniParserTest, TrimSectionKeyAndValue)
{
    WriteConfig("  [ section ]  \n  key  =  value  \n");

    IniParser parser;
    ASSERT_EQ(parser.Load(TEST_INI_PATH), 0);
    EXPECT_EQ(parser.GetValue("section", "key"), "value");
}

// 测试 value 中包含额外等号时保留等号及其后内容
TEST_F(IniParserTest, PreserveValueAfterAdditionalDelimiter)
{
    WriteConfig("[section]\npath=/tmp/a=b\n");

    IniParser parser;
    ASSERT_EQ(parser.Load(TEST_INI_PATH), 0);
    EXPECT_EQ(parser.GetValue("section", "path"), "/tmp/a=b");
}

// 测试同一 section 中重复 key 时后读取的 value 覆盖前值
TEST_F(IniParserTest, LastValueWinsForDuplicateKey)
{
    WriteConfig("[section]\nkey=first\nkey=second\n");

    IniParser parser;
    ASSERT_EQ(parser.Load(TEST_INI_PATH), 0);
    EXPECT_EQ(parser.GetValue("section", "key"), "second");
}

// 测试读取不存在的 key 或 section 时返回默认值
TEST_F(IniParserTest, ReturnDefaultForMissingValue)
{
    WriteConfig("[section]\nkey=value\n");

    IniParser parser;
    ASSERT_EQ(parser.Load(TEST_INI_PATH), 0);
    EXPECT_EQ(parser.GetValue("section", "missing"), "");
    EXPECT_EQ(parser.GetValue("missing", "key", "fallback"), "fallback");
}

// 测试重新加载配置后旧 section 和旧 key 不会残留
TEST_F(IniParserTest, ReloadReplacesPreviousContent)
{
    WriteConfig("[old]\nkey=value\n");

    IniParser parser;
    ASSERT_EQ(parser.Load(TEST_INI_PATH), 0);
    ASSERT_EQ(parser.GetSections().size(), 1U);

    WriteConfig("[new]\nkey=value\n");
    ASSERT_EQ(parser.Load(TEST_INI_PATH), 0);
    EXPECT_EQ(parser.GetValue("old", "key", "missing"), "missing");
    EXPECT_EQ(parser.GetValue("new", "key"), "value");
}

// 测试获取不存在的 section 时返回空容器
TEST_F(IniParserTest, MissingSectionReturnsEmptyContainer)
{
    WriteConfig("[section]\nkey=value\n");

    IniParser parser;
    ASSERT_EQ(parser.Load(TEST_INI_PATH), 0);
    EXPECT_TRUE(parser.GetSection("missing").empty());
}

// 测试配置文件不存在时返回加载错误
TEST_F(IniParserTest, LoadMissingFileReturnsError)
{
    IniParser parser;
    EXPECT_EQ(parser.Load("/tmp/sparrow_ini_parser_missing.ini"), -1);
}

// 测试 section 之前的 key/value 配置不会被保存
TEST_F(IniParserTest, IgnoreKeyBeforeSection)
{
    WriteConfig(
        "key=value\n"
        "[section]\n"
        "valid=yes\n");

    IniParser parser;
    ASSERT_EQ(parser.Load(TEST_INI_PATH), 0);
    EXPECT_EQ(parser.GetValue("", "key", "missing"), "missing");
    EXPECT_EQ(parser.GetValue("section", "valid"), "yes");
}

// 测试 ForEachSection 遍历所有 section
TEST_F(IniParserTest, ForEachSectionVisitsAllSections)
{
    WriteConfig(
        "[logging]\n"
        "level=debug\n"
        "[output]\n"
        "path=/tmp/log\n");

    IniParser parser;
    ASSERT_EQ(parser.Load(TEST_INI_PATH), 0);

    std::vector<std::string> sections;
    parser.ForEachSection([&sections](const std::string& section) {
        sections.push_back(section);
        return true;
    });

    ASSERT_EQ(sections.size(), 2U);
    EXPECT_EQ(sections[0], "logging");
    EXPECT_EQ(sections[1], "output");
}

// 测试 ForEachKey 遍历指定 section 中的所有 key/value
TEST_F(IniParserTest, ForEachKeyVisitsKeysInSection)
{
    WriteConfig("[section]\nb=2\na=1\n");

    IniParser parser;
    ASSERT_EQ(parser.Load(TEST_INI_PATH), 0);

    std::vector<std::string> values;
    parser.ForEachKey("section", [&values](const std::string& key, const std::string& value) {
        values.push_back(key + "=" + value);
        return true;
    });

    ASSERT_EQ(values.size(), 2U);
    EXPECT_EQ(values[0], "a=1");
    EXPECT_EQ(values[1], "b=2");
}

// 测试 ForEach 遍历配置中的所有 section、key 和 value
TEST_F(IniParserTest, ForEachAllVisitsEveryEntry)
{
    WriteConfig(
        "[first]\n"
        "key=value\n"
        "[second]\n"
        "other=another\n");

    IniParser parser;
    ASSERT_EQ(parser.Load(TEST_INI_PATH), 0);

    std::vector<std::string> entries;
    parser.ForEach([&entries](const std::string& section,
                                 const std::string& key,
                                 const std::string& value) {
        entries.push_back(section + "." + key + "=" + value);
        return true;
    });

    ASSERT_EQ(entries.size(), 2U);
    EXPECT_EQ(entries[0], "first.key=value");
    EXPECT_EQ(entries[1], "second.other=another");
}

// 测试遍历回调返回 false 时立即停止遍历
TEST_F(IniParserTest, ForEachStopsWhenCallbackReturnsFalse)
{
    WriteConfig(
        "[first]\n"
        "a=1\n"
        "b=2\n"
        "[second]\n"
        "c=3\n");

    IniParser parser;
    ASSERT_EQ(parser.Load(TEST_INI_PATH), 0);

    int sectionCount = 0;
    parser.ForEachSection([&sectionCount](const std::string&) {
        ++sectionCount;
        return false;
    });
    EXPECT_EQ(sectionCount, 1);

    int keyCount = 0;
    parser.ForEachKey("first", [&keyCount](const std::string&, const std::string&) {
        ++keyCount;
        return false;
    });
    EXPECT_EQ(keyCount, 1);

    int entryCount = 0;
    parser.ForEach([&entryCount](const std::string&, const std::string&, const std::string&) {
        ++entryCount;
        return false;
    });
    EXPECT_EQ(entryCount, 1);
}

// 测试遍历不存在的 section 时不会触发回调
TEST_F(IniParserTest, ForEachMissingSectionDoesNothing)
{
    WriteConfig("[section]\nkey=value\n");

    IniParser parser;
    ASSERT_EQ(parser.Load(TEST_INI_PATH), 0);

    int count = 0;
    parser.ForEachKey("missing", [&count](const std::string&, const std::string&) {
        ++count;
        return true;
    });
    EXPECT_EQ(count, 0);
}
