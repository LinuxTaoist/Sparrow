/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SampleSqlite.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/05/05
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/05 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <iostream>
#include <stdio.h>
#include "SqliteAdapter.h"

#define SPR_LOGD(fmt, args...) printf("%d SampleSqlite D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d SampleSqlite E: " fmt, __LINE__, ##args)
int main(int argc, const char *argv[])
{
    // 初始化数据库适配器，假设数据库文件路径为"data.db"
    SqliteAdapter* pSqlAdapter = SqliteAdapter::GetInstance("data.db");

    // 创建表（仅作为示例，实际使用时可能需要先检查表是否存在）
    pSqlAdapter->Remove("users"); // 如果表已存在，先删除以便重新创建
    std::map<std::string, std::string> columnsDef {
        {"id", "INTEGER PRIMARY KEY"},
        {"name", "TEXT NOT NULL"},
        {"age", "INTEGER"}
    };

    pSqlAdapter->CreateTable("users", columnsDef);
    pSqlAdapter->Insert("users", {{"name", "Alice"}, {"age", "30"}}); // 示例插入数据，id让SQLite自动生成

    // 插入数据
    pSqlAdapter->Insert("users", {{"name", "Bob"}, {"age", "25"}});

    // 查询数据
    std::vector<std::vector<std::string>> result = pSqlAdapter->Query("users");
    for (const auto& row : result) {
        SPR_LOGD("ID: %s, Name: %s, Age: %s\n", row[0].c_str(), row[1].c_str(), row[2].c_str());
    }

    // 更新数据
    pSqlAdapter->Update("users", {{"age", "31"}}, "name='Alice'");

    // 再次查询，查看更新效果
    result = pSqlAdapter->Query("users");
    SPR_LOGD("\nAfter Update:\n");
    for (const auto& row : result) {
        SPR_LOGD("ID: %s, Name: %s, Age: %s\n", row[0].c_str(), row[1].c_str(), row[2].c_str());
    }

    // 删除数据
    pSqlAdapter->Remove("users", "name='Bob'");

    // 最终查询，验证删除
    result = pSqlAdapter->Query("users");
    SPR_LOGD("\nAfter Delete:\n");
    for (const auto& row : result) {
        SPR_LOGD("ID: %s, Name: %s, Age: %s\n", row[0].c_str(), row[1].c_str(), row[2].c_str());
    }

    return 0;
}

