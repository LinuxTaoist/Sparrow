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

#define SPR_LOGD(fmt, args...) printf("%d DebugBinder D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d DebugBinder E: " fmt, __LINE__, ##args)
int main() {
    try {
        // 初始化数据库适配器，假设数据库文件路径为"data.db"
        SqliteAdapter adapter("data.db");

        // 创建表（仅作为示例，实际使用时可能需要先检查表是否存在）
        adapter.Remove("users"); // 如果表已存在，先删除以便重新创建
        std::string createTable = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER);";
        std::map<std::string, std::string> columnsDef {
            {"id", "INTEGER PRIMARY KEY"},
            {"name", "TEXT NOT NULL"},
            {"age", "INTEGER"}
        };

        // adapter.Execute(createTable);
        adapter.CreateTable("users", columnsDef);
        adapter.Insert("users", {{"name", "Alice"}, {"age", "30"}}); // 示例插入数据，id让SQLite自动生成

        // 插入数据
        adapter.Insert("users", {{"name", "Bob"}, {"age", "25"}});

        // 查询数据
        std::vector<std::vector<std::string>> result = adapter.Query("users");
        for (const auto& row : result) {
            std::cout << "ID: " << row[0] << ", Name: " << row[1] << ", Age: " << row[2] << std::endl;
        }

        // 更新数据
        adapter.Update("users", {{"age", "31"}}, "name='Alice'");

        // 再次查询，查看更新效果
        result = adapter.Query("users");
        std::cout << "\nAfter Update:\n";
        for (const auto& row : result) {
            std::cout << "ID: " << row[0] << ", Name: " << row[1] << ", Age: " << row[2] << std::endl;
        }

        // 删除数据
        adapter.Remove("users", "name='Bob'");

        // 最终查询，验证删除
        result = adapter.Query("users");
        std::cout << "\nAfter Delete:\n";
        for (const auto& row : result) {
            std::cout << "ID: " << row[0] << ", Name: " << row[1] << ", Age: " << row[2] << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

