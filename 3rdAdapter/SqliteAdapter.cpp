/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SqliteAdapter.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
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
#include <vector>
#include <atomic>
#include "SqliteAdapter.h"

#define SPR_LOGD(fmt, args...) printf("%d SQLAdapter D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%d SQLAdapter W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d SQLAdapter E: " fmt, __LINE__, ##args)

static sqlite3* pDb = nullptr;
static std::atomic<bool> gObjAlive(true);

static int callback(void* data, int argc, char** argv, char** azColName)
{
    std::vector<std::vector<std::string>>* results = static_cast<std::vector<std::vector<std::string>>*>(data);
    if (!results) return SQLITE_ABORT;

    std::vector<std::string> row;
    for (int i = 0; i < argc; ++i) {
        row.push_back(argv[i] ? argv[i] : "NULL");
    }
    results->push_back(row);
    return 0;
}

bool SqliteAdapter::Execute(const std::string& sqlStr)
{
    char* errMsg;
    int rc = sqlite3_exec(pDb, sqlStr.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        SPR_LOGE("SQL error: [%s] (%s)\n", sqlStr.c_str(), errMsg);
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}
SqliteAdapter::SqliteAdapter(const std::string& dbPath)
{
    int rc = sqlite3_open(dbPath.c_str(), &pDb);
    if (rc) {
        SPR_LOGE("Open %s fail! (%s)\n", dbPath.c_str(), sqlite3_errmsg(pDb));
    }
}

SqliteAdapter::~SqliteAdapter()
{
    gObjAlive = false;
    if (pDb) {
        sqlite3_close(pDb);
    }
}

SqliteAdapter* SqliteAdapter::GetInstance(const std::string& dbPath)
{
    if (!gObjAlive) {
        return nullptr;
    }

    static SqliteAdapter instance(dbPath);
    return &instance;
}

bool SqliteAdapter::CreateTable(const std::string& tableName, const std::map<std::string, std::string>& columnsDefinition)
{
    std::string columnDefs;
    for (const auto& col : columnsDefinition) {
        if (!columnDefs.empty()) columnDefs += ", ";
        columnDefs += col.first + " " + col.second;
    }

    std::string sqlStr = "CREATE TABLE IF NOT EXISTS " + tableName + " (" + columnDefs + ");";
    char* errMsg;
    int rc = sqlite3_exec(pDb, sqlStr.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool SqliteAdapter::Insert(const std::string& table, const std::vector<std::pair<std::string, std::string>>& columnsValues)
{
    std::string columns, values;
    for (const auto& pair : columnsValues) {
        if (!columns.empty()) {
            columns += ", ";
            values += ", ";
        }
        columns += pair.first;
        values += "'" + pair.second + "'";
    }

    char* errMsg;
    std::string sqlStr = "INSERT INTO " + table + " (" + columns + ") VALUES (" + values + ");";
    int rc = sqlite3_exec(pDb, sqlStr.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        SPR_LOGE("SQL error: [%s] (%s)\n", sqlStr.c_str(), errMsg);
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool SqliteAdapter::Remove(const std::string& table, const std::string& condition)
{
    std::string sqlStr = "DELETE FROM " + table + (condition.empty() ? "" : " WHERE " + condition);
    char* errMsg;

    int rc = sqlite3_exec(pDb, sqlStr.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        SPR_LOGW("SQL: [%s] (%s)\n", sqlStr.c_str(), errMsg);
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool SqliteAdapter::Update(const std::string& table, const std::vector<std::pair<std::string, std::string>>& columnsValues, const std::string& condition)
{
    std::string setClause;
    for (size_t i = 0; i < columnsValues.size(); ++i) {
        if (i > 0) setClause += ", ";
        setClause += columnsValues[i].first + " = '" + columnsValues[i].second + "'";
    }

    std::string sqlStr = "UPDATE " + table + " SET " + setClause + (condition.empty() ? "" : " WHERE " + condition);
    char* errMsg;
    int rc = sqlite3_exec(pDb, sqlStr.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        SPR_LOGE("SQL error: [%s] (%s)\n", sqlStr.c_str(), errMsg);
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

std::vector<std::vector<std::string>> SqliteAdapter::Query(const std::string& table, const std::string& columns, const std::string& condition)
{
    std::string sqlStr = "SELECT " + columns + " FROM " + table + (condition.empty() ? "" : " WHERE " + condition);
    std::vector<std::vector<std::string>> results;
    char* errMsg;

    int rc = sqlite3_exec(pDb, sqlStr.c_str(), callback, &results, &errMsg);
    if (rc != SQLITE_OK) {
        SPR_LOGE("SQL error: [%s] (%s)\n", sqlStr.c_str(), errMsg);
        sqlite3_free(errMsg);
    }

    return results;
}
