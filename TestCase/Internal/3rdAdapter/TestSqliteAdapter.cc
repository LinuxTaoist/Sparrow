/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSqliteAdapter.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SqliteAdapter SQLite 适配器内部测试
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
#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <vector>
#include <map>
#include "SqliteAdapter.h"
#include "gtest/gtest.h"

class Util_SqliteAdapter : public ::testing::Test {
protected:
    void SetUp() override {
        mpDb = SqliteAdapter::GetInstance(":memory:");
    }

    void TearDown() override {
        unlink("/tmp/spr_sqlite_bak_test.db");
    }

    SqliteAdapter* mpDb;
};

// 测试单例模式返回非空
TEST_F(Util_SqliteAdapter, GetInstanceReturnsValid)
{
    ASSERT_TRUE(mpDb != nullptr);
    EXPECT_EQ(mpDb, SqliteAdapter::GetInstance(":memory:"));
}

// 测试建表并插入数据
TEST_F(Util_SqliteAdapter, CreateTableAndInsert)
{
    std::map<std::string, std::string> cols = {
        {"id", "INTEGER PRIMARY KEY"},
        {"name", "TEXT"},
    };
    EXPECT_TRUE(mpDb->CreateTable("t_users", cols));

    std::vector<std::pair<std::string, std::string>> values = {
        {"id", "1"},
        {"name", "alice"},
    };
    EXPECT_TRUE(mpDb->Insert("t_users", values));
}

// 测试查询数据（带列名重载）
TEST_F(Util_SqliteAdapter, QueryByTableAndColumns)
{
    std::map<std::string, std::string> cols = {{"id", "INTEGER"}, {"val", "TEXT"}};
    ASSERT_TRUE(mpDb->CreateTable("t_query", cols));

    std::vector<std::pair<std::string, std::string>> v1 = {{"id", "1"}, {"val", "hello"}};
    std::vector<std::pair<std::string, std::string>> v2 = {{"id", "2"}, {"val", "world"}};
    ASSERT_TRUE(mpDb->Insert("t_query", v1));
    ASSERT_TRUE(mpDb->Insert("t_query", v2));

    std::vector<std::vector<std::string>> rows;
    EXPECT_TRUE(mpDb->Query("t_query", "*", rows));
    EXPECT_EQ(rows.size(), 2u);
    EXPECT_EQ(rows[0][1], "hello");
    EXPECT_EQ(rows[1][1], "world");
}

// 测试查询数据（原始 SQL 重载）
TEST_F(Util_SqliteAdapter, QueryByRawSql)
{
    std::map<std::string, std::string> cols = {{"id", "INTEGER"}, {"val", "TEXT"}};
    ASSERT_TRUE(mpDb->CreateTable("t_raw", cols));
    std::vector<std::pair<std::string, std::string>> v = {{"id", "1"}, {"val", "abc"}};
    ASSERT_TRUE(mpDb->Insert("t_raw", v));

    std::vector<std::vector<std::string>> rows;
    EXPECT_TRUE(mpDb->Query("SELECT val FROM t_raw WHERE id = 1", rows));
    ASSERT_EQ(rows.size(), 1u);
    EXPECT_EQ(rows[0][0], "abc");
}

// 测试更新数据
TEST_F(Util_SqliteAdapter, Update)
{
    std::map<std::string, std::string> cols = {{"id", "INTEGER"}, {"val", "TEXT"}};
    ASSERT_TRUE(mpDb->CreateTable("t_update", cols));
    std::vector<std::pair<std::string, std::string>> v = {{"id", "1"}, {"val", "before"}};
    ASSERT_TRUE(mpDb->Insert("t_update", v));

    std::vector<std::pair<std::string, std::string>> update = {{"val", "after"}};
    EXPECT_TRUE(mpDb->Update("t_update", update, "id = 1"));

    std::vector<std::vector<std::string>> rows;
    ASSERT_TRUE(mpDb->Query("SELECT val FROM t_update WHERE id = 1", rows));
    EXPECT_EQ(rows[0][0], "after");
}

// 测试删除数据
TEST_F(Util_SqliteAdapter, Remove)
{
    std::map<std::string, std::string> cols = {{"id", "INTEGER"}, {"val", "TEXT"}};
    ASSERT_TRUE(mpDb->CreateTable("t_remove", cols));
    std::vector<std::pair<std::string, std::string>> v = {{"id", "1"}, {"val", "x"}};
    ASSERT_TRUE(mpDb->Insert("t_remove", v));

    EXPECT_TRUE(mpDb->Remove("t_remove", "id = 1"));

    std::vector<std::vector<std::string>> rows;
    ASSERT_TRUE(mpDb->Query("SELECT * FROM t_remove", rows));
    EXPECT_TRUE(rows.empty());
}

// 测试执行任意 SQL
TEST_F(Util_SqliteAdapter, Execute)
{
    EXPECT_TRUE(mpDb->Execute("CREATE TABLE IF NOT EXISTS t_exec (id INTEGER)"));
    EXPECT_TRUE(mpDb->Execute("INSERT INTO t_exec (id) VALUES (42)"));

    std::vector<std::vector<std::string>> rows;
    ASSERT_TRUE(mpDb->Query("SELECT * FROM t_exec", rows));
    EXPECT_EQ(rows.size(), 1u);
    EXPECT_EQ(rows[0][0], "42");
}

// 测试备份到文件
TEST_F(Util_SqliteAdapter, BackupTo)
{
    std::map<std::string, std::string> cols = {{"id", "INTEGER"}, {"val", "TEXT"}};
    ASSERT_TRUE(mpDb->CreateTable("t_backup", cols));
    std::vector<std::pair<std::string, std::string>> v = {{"id", "1"}, {"val", "data"}};
    ASSERT_TRUE(mpDb->Insert("t_backup", v));

    const std::string backupPath = "/tmp/spr_sqlite_bak_test.db";
    unlink(backupPath.c_str());
    EXPECT_TRUE(mpDb->BackupTo(backupPath));
    EXPECT_EQ(access(backupPath.c_str(), F_OK), 0);

    unlink(backupPath.c_str());
}

// 测试执行错误 SQL 返回失败
TEST_F(Util_SqliteAdapter, ExecuteInvalidSqlFails)
{
    EXPECT_FALSE(mpDb->Execute("NOT A VALID SQL STATEMENT"));
    std::vector<std::vector<std::string>> rows;
    EXPECT_FALSE(mpDb->Query("SELECT FROM nonexistent_table_xyz", rows));
}
