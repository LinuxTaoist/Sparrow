/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SqliteAdapter.h
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
#ifndef __SQLITE_ADAPTER_H__
#define __SQLITE_ADAPTER_H__

#include "sqlite3.h"
#include <map>
#include <string>
#include <vector>

class SqliteAdapter {
public:
    ~SqliteAdapter();
    static SqliteAdapter* GetInstance(const std::string& dbPath);

    bool CreateTable(const std::string& tableName, const std::map<std::string, std::string>& columnsDefinition);
    bool Execute(const std::string& sqlStr);
    bool Insert(const std::string& table, const std::vector<std::pair<std::string, std::string>>& columnsValues);
    bool Remove(const std::string& table, const std::string& condition = "");
    bool Update(const std::string& table, const std::vector<std::pair<std::string, std::string>>& columnsValues, const std::string& condition = "");
    std::vector<std::vector<std::string>> Query(const std::string& table, const std::string& columns = "*", const std::string& condition = "");

private:
    explicit SqliteAdapter(const std::string& dbPath);
};

#endif // __SQLITE_ADAPTER_H__
