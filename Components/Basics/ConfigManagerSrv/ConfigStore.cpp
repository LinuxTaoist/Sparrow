/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ConfigStore.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/06/20
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/06/20 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include "SprLog.h"
#include "CommonMacros.h"
#include "GeneralUtils.h"
#include "CommonTypeDefs.h"
#include "CoreTypeDefs.h"
#include "ConfigStore.h"
#include "SqliteAdapter.h"

using namespace InternalDefs;

#define LOG_TAG "CfgStore"

static const char* META_TABLE = "meta_info";
static const char* DEFAULT_TABLE = "default_items";
static const char* FACTORY_TABLE = "factory_items";
static const char* USER_TABLE = "user_items";

std::string GetNowString()
{
    return GeneralUtils::GetCurTimeStr();
}

static std::string SqlEscape(const std::string& value)
{
    std::string escaped;
    escaped.reserve(value.size());
    for (char ch : value) {
        if (ch == '\'') {
            escaped += "''";
        } else {
            escaped += ch;
        }
    }
    return escaped;
}

static std::string SqlText(const std::string& value)
{
    return "'" + SqlEscape(value) + "'";
}

static std::string HexEncode(const std::vector<uint8_t>& value)
{
    static const char kHex[] = "0123456789ABCDEF";
    std::string out;
    out.reserve(value.size() * 2);
    for (uint8_t byte : value) {
        out.push_back(kHex[(byte >> 4) & 0x0F]);
        out.push_back(kHex[byte & 0x0F]);
    }
    return out;
}

static std::vector<uint8_t> HexDecode(const std::string& value)
{
    std::vector<uint8_t> out;
    if (value.empty()) {
        return out;
    }

    std::string text = value;
    if (text.size() >= 2 && text[0] == 'X' && text[1] == '\'') {
        text = text.substr(2);
    }
    if (text.size() % 2 != 0) {
        text = text.substr(0, text.size() - 1);
    }

    out.reserve(text.size() / 2);
    for (size_t i = 0; i + 1 < text.size(); i += 2) {
        char hi = text[i];
        char lo = text[i + 1];
        auto HexToByte = [](char ch) -> uint8_t {
            if (ch >= '0' && ch <= '9') return static_cast<uint8_t>(ch - '0');
            if (ch >= 'a' && ch <= 'f') return static_cast<uint8_t>(10 + (ch - 'a'));
            if (ch >= 'A' && ch <= 'F') return static_cast<uint8_t>(10 + (ch - 'A'));
            return 0;
        };
        out.push_back(static_cast<uint8_t>((HexToByte(hi) << 4) | HexToByte(lo)));
    }
    return out;
}

static bool HasColumn(SqliteAdapter* db, const char* tableName, const char* columnName)
{
    if (db == nullptr) {
        return false;
    }
    std::string sql = std::string("PRAGMA table_info(") + tableName + ");";
    std::vector<std::vector<std::string>> rows;
    if (!db->Query(sql, rows)) {
        return false;
    }
    for (const auto& row : rows) {
        if (row.size() > 1 && row[1] == columnName) {
            return true;
        }
    }
    return false;
}

static int32_t EnsureItemTypeColumn(SqliteAdapter* db, const char* tableName)
{
    if (HasColumn(db, tableName, "item_type")) {
        return 0;
    }

    std::string sql = std::string("ALTER TABLE ") + tableName +
                      " ADD COLUMN item_type INTEGER NOT NULL DEFAULT 0;";
    return db->Execute(sql) ? 0 : -1;
}

ConfigStore::ConfigStore(const std::string& dbPath)
    : mpDb(SqliteAdapter::GetInstance(dbPath)), mDbPath(dbPath), mNextRevision(0)
{
}

ConfigStore::~ConfigStore()
{
    mpDb = nullptr;
}

int32_t ConfigStore::Initialize()
{
    if (mpDb == nullptr) {
        mpDb = SqliteAdapter::GetInstance(mDbPath);
    }
    if (mpDb == nullptr) {
        SPR_LOGE("Open db failed: %s\n", mDbPath.c_str());
        return -1;
    }

    NONZERO_CHECK_RET(mpDb->Execute("PRAGMA journal_mode=WAL;") ? 0 : -1);
    NONZERO_CHECK_RET(mpDb->Execute("PRAGMA synchronous=NORMAL;") ? 0 : -1);
    NONZERO_CHECK_RET(CreateTables());
    NONZERO_CHECK_RET(LoadNextRevision());
    return 0;
}

int32_t ConfigStore::CreateTables()
{
    if (mpDb == nullptr) {
        return -1;
    }

    std::string metaSql = std::string(
        "CREATE TABLE IF NOT EXISTS ") + META_TABLE +
        " ("
        "meta_key TEXT PRIMARY KEY NOT NULL,"
        "meta_value TEXT NOT NULL);";
    std::string itemSql =
        "CREATE TABLE IF NOT EXISTS %s ("
        "namespace_name TEXT NOT NULL,"
        "item_key TEXT NOT NULL,"
        "item_value BLOB NOT NULL,"
        "item_type INTEGER NOT NULL DEFAULT 0,"
        "revision INTEGER NOT NULL,"
        "schema_version INTEGER NOT NULL DEFAULT 1,"
        "owner TEXT NOT NULL DEFAULT '',"
        "update_time TEXT NOT NULL,"
        "PRIMARY KEY(namespace_name, item_key));";

    NONZERO_CHECK_RET(mpDb->Execute(metaSql) ? 0 : -1);
    std::string defaultSql = std::string("CREATE TABLE IF NOT EXISTS ") + DEFAULT_TABLE + " ("
        "namespace_name TEXT NOT NULL,"
        "item_key TEXT NOT NULL,"
        "item_value BLOB NOT NULL,"
        "item_type INTEGER NOT NULL DEFAULT 0,"
        "revision INTEGER NOT NULL,"
        "schema_version INTEGER NOT NULL DEFAULT 1,"
        "owner TEXT NOT NULL DEFAULT '',"
        "update_time TEXT NOT NULL,"
        "PRIMARY KEY(namespace_name, item_key));";
    std::string factorySql = std::string("CREATE TABLE IF NOT EXISTS ") + FACTORY_TABLE + " ("
        "namespace_name TEXT NOT NULL,"
        "item_key TEXT NOT NULL,"
        "item_value BLOB NOT NULL,"
        "item_type INTEGER NOT NULL DEFAULT 0,"
        "revision INTEGER NOT NULL,"
        "schema_version INTEGER NOT NULL DEFAULT 1,"
        "owner TEXT NOT NULL DEFAULT '',"
        "update_time TEXT NOT NULL,"
        "PRIMARY KEY(namespace_name, item_key));";
    std::string userSql = std::string("CREATE TABLE IF NOT EXISTS ") + USER_TABLE + " ("
        "namespace_name TEXT NOT NULL,"
        "item_key TEXT NOT NULL,"
        "item_value BLOB NOT NULL,"
        "item_type INTEGER NOT NULL DEFAULT 0,"
        "revision INTEGER NOT NULL,"
        "schema_version INTEGER NOT NULL DEFAULT 1,"
        "owner TEXT NOT NULL DEFAULT '',"
        "update_time TEXT NOT NULL,"
        "PRIMARY KEY(namespace_name, item_key));";

    int32_t ret = 0;
    ret = mpDb->Execute(defaultSql) ? 0 : -1;
    if (ret == 0) {
        ret = mpDb->Execute(factorySql) ? 0 : -1;
    }
    if (ret == 0) {
        ret = mpDb->Execute(userSql) ? 0 : -1;
    }
    if (ret != 0) {
        return -1;
    }

    NONZERO_CHECK_RET(EnsureItemTypeColumn(mpDb, DEFAULT_TABLE));
    NONZERO_CHECK_RET(EnsureItemTypeColumn(mpDb, FACTORY_TABLE));
    NONZERO_CHECK_RET(EnsureItemTypeColumn(mpDb, USER_TABLE));

    return 0;
}

int32_t ConfigStore::QueryMaxRevision(const std::string& tableName, int32_t& maxRevision)
{
    if (mpDb == nullptr) {
        return -1;
    }

    maxRevision = 0;
    std::string sql = "SELECT IFNULL(MAX(revision), 0) FROM " + tableName + ";";
    std::vector<std::vector<std::string>> rows;
    if (!mpDb->Query(sql, rows) || rows.empty() || rows[0].empty()) {
        return -1;
    }
    maxRevision = std::stoi(rows[0][0]);
    return 0;
}

int32_t ConfigStore::LoadNextRevision()
{
    if (mpDb == nullptr) {
        return -1;
    }

    std::string sql = std::string("SELECT meta_value FROM ") + META_TABLE +
                      " WHERE meta_key='next_revision';";
    std::vector<std::vector<std::string>> rows;
    if (mpDb->Query(sql, rows) && !rows.empty() && !rows[0].empty()) {
        mNextRevision = std::stoi(rows[0][0]);
        if (mNextRevision > 0) {
            return 0;
        }
    }

    int32_t maxRevision = 0;
    int32_t tempRevision = 0;
    NONZERO_CHECK_RET(QueryMaxRevision(DEFAULT_TABLE, maxRevision));
    NONZERO_CHECK_RET(QueryMaxRevision(FACTORY_TABLE, tempRevision));
    if (tempRevision > maxRevision) {
        maxRevision = tempRevision;
    }
    NONZERO_CHECK_RET(QueryMaxRevision(USER_TABLE, tempRevision));
    if (tempRevision > maxRevision) {
        maxRevision = tempRevision;
    }

    mNextRevision = maxRevision + 1;
    return UpdateNextRevision(mNextRevision);
}

int32_t ConfigStore::UpdateNextRevision(int32_t revision)
{
    if (mpDb == nullptr) {
        return -1;
    }

    std::string sql = std::string("INSERT OR REPLACE INTO ") + META_TABLE +
        "(meta_key, meta_value) VALUES('next_revision', " + SqlText(std::to_string(revision)) + ");";
    return mpDb->Execute(sql) ? 0 : -1;
}

int32_t ConfigStore::BeginTransaction()
{
    return (mpDb && mpDb->Execute("BEGIN IMMEDIATE TRANSACTION;")) ? 0 : -1;
}

int32_t ConfigStore::CommitTransaction()
{
    return (mpDb && mpDb->Execute("COMMIT;")) ? 0 : -1;
}

int32_t ConfigStore::RollbackTransaction()
{
    return (mpDb && mpDb->Execute("ROLLBACK;")) ? 0 : -1;
}

std::string ConfigStore::TableName(int32_t scope) const
{
    switch (scope) {
        case CONFIG_SCOPE_DEFAULT:
            return DEFAULT_TABLE;
        case CONFIG_SCOPE_FACTORY:
            return FACTORY_TABLE;
        case CONFIG_SCOPE_USER:
            return USER_TABLE;
        default:
            return "";
    }
}

int32_t ConfigStore::Upsert(int32_t scope, const std::string& nameSpace, const std::string& key,
                            const std::string& value, const std::string& owner, int32_t& revision)
{
    std::vector<uint8_t> rawValue(value.begin(), value.end());
    return UpsertRaw(scope, nameSpace, key, rawValue, CONFIG_VALUE_TYPE_STRING, owner, revision);
}

int32_t ConfigStore::UpsertRaw(int32_t scope, const std::string& nameSpace, const std::string& key,
                               const std::vector<uint8_t>& value, int32_t valueType,
                               const std::string& owner, int32_t& revision)
{
    if (mpDb == nullptr) {
        return -1;
    }
    const std::string tableName = TableName(scope);
    if (tableName.empty()) {
        return -1;
    }

    NONZERO_CHECK_RET(BeginTransaction());
    revision = mNextRevision;
    mNextRevision++;

    std::string now = GetNowString();
    std::string blobLiteral = "X'" + HexEncode(value) + "'";
    std::string sql = std::string("INSERT OR REPLACE INTO ") + tableName +
        "(namespace_name, item_key, item_value, item_type, revision, schema_version, owner, update_time) "
        "VALUES(" + SqlText(nameSpace) + ", " + SqlText(key) + ", " + blobLiteral + ", " +
        std::to_string(valueType) + ", " + std::to_string(revision) + ", 1, " + SqlText(owner) + ", " +
        SqlText(now) + ");";

    if (!mpDb->Execute(sql) || UpdateNextRevision(mNextRevision) != 0) {
        RollbackTransaction();
        mNextRevision = revision;
        return -1;
    }
    if (CommitTransaction() != 0) {
        RollbackTransaction();
        mNextRevision = revision;
        return -1;
    }
    return 0;
}

int32_t ConfigStore::EnsureValue(int32_t scope, const std::string& nameSpace, const std::string& key,
                                 const std::string& value, const std::string& owner, int32_t& revision)
{
    ConfigRecord record;
    if (GetByScope(scope, nameSpace, key, record) == 0) {
        revision = record.revision;
        return 0;
    }

    return Upsert(scope, nameSpace, key, value, owner, revision);
}

int32_t ConfigStore::Remove(int32_t scope, const std::string& nameSpace, const std::string& key, int32_t& revision)
{
    if (mpDb == nullptr) {
        return -1;
    }
    const std::string tableName = TableName(scope);
    if (tableName.empty()) {
        return -1;
    }

    NONZERO_CHECK_RET(BeginTransaction());
    revision = mNextRevision;
    mNextRevision++;
    std::string sql = std::string("DELETE FROM ") + tableName + " WHERE namespace_name=" +
        SqlText(nameSpace) + " AND item_key=" + SqlText(key) + ";";
    if (!mpDb->Execute(sql) || UpdateNextRevision(mNextRevision) != 0) {
        RollbackTransaction();
        mNextRevision = revision;
        return -1;
    }
    if (CommitTransaction() != 0) {
        RollbackTransaction();
        mNextRevision = revision;
        return -1;
    }
    return 0;
}

int32_t ConfigStore::QueryTable(const std::string& tableName, const std::string& nameSpace,
                                const std::string& key, ConfigRecord& record)
{
    if (mpDb == nullptr) {
        return -1;
    }

    std::string sql = "SELECT item_value, item_type, revision, owner, update_time FROM " + tableName +
        " WHERE namespace_name=" + SqlText(nameSpace) + " AND item_key=" + SqlText(key) + ";";
    std::vector<std::vector<std::string>> rows;
    if (!mpDb->Query(sql, rows) || rows.empty() || rows[0].size() < 5) {
        return -1;
    }

    const std::string& blobText = rows[0][0];
    record.rawValue = HexDecode(blobText);
    if (record.rawValue.empty() && !blobText.empty()) {
        record.rawValue.assign(blobText.begin(), blobText.end());
    }

    record.valueType = std::stoi(rows[0][1]);
    if (record.valueType == CONFIG_VALUE_TYPE_STRING) {
        record.value.assign(record.rawValue.begin(), record.rawValue.end());
    } else {
        record.value.clear();
    }

    record.revision = std::stoi(rows[0][2]);
    record.owner = rows[0][3];
    record.updateTime = rows[0][4];
    record.scope = 0;
    return 0;
}

int32_t ConfigStore::GetByScope(int32_t scope, const std::string& nameSpace, const std::string& key, ConfigRecord& record)
{
    const std::string tableName = TableName(scope);
    if (tableName.empty()) {
        return -1;
    }

    record.scope = scope;
    return QueryTable(tableName, nameSpace, key, record);
}

int32_t ConfigStore::GetByScopeRaw(int32_t scope, const std::string& nameSpace, const std::string& key, ConfigRecord& record)
{
    return GetByScope(scope, nameSpace, key, record);
}

int32_t ConfigStore::GetEffective(const std::string& nameSpace, const std::string& key, ConfigRecord& record)
{
    int32_t ret = GetByScope(CONFIG_SCOPE_USER, nameSpace, key, record);
    if (ret == 0) {
        return 0;
    }

    ret = GetByScope(CONFIG_SCOPE_FACTORY, nameSpace, key, record);
    if (ret == 0) {
        return 0;
    }

    return GetByScope(CONFIG_SCOPE_DEFAULT, nameSpace, key, record);
}

int32_t ConfigStore::GetEffectiveRaw(const std::string& nameSpace, const std::string& key, ConfigRecord& record)
{
    return GetEffective(nameSpace, key, record);
}

int32_t ConfigStore::QueryNamespace(const std::string& tableName, const std::string& nameSpace,
                                    std::map<std::string, std::string>& items)
{
    if (mpDb == nullptr) {
        return -1;
    }

    std::string sql = "SELECT item_key, item_value, item_type FROM " + tableName +
        " WHERE namespace_name=" + SqlText(nameSpace) + ";";
    std::vector<std::vector<std::string>> rows;
    if (!mpDb->Query(sql, rows)) {
        return -1;
    }
    for (const auto& row : rows) {
        if (row.size() < 3) {
            continue;
        }
        int valueType = std::stoi(row[2]);
        if (valueType == CONFIG_VALUE_TYPE_STRING) {
            items[row[0]] = row[1];
        }
    }
    return 0;
}

int32_t ConfigStore::ListNamespaceEffective(const std::string& nameSpace, std::map<std::string, std::string>& items)
{
    items.clear();
    NONZERO_CHECK_RET(QueryNamespace(DEFAULT_TABLE, nameSpace, items));
    NONZERO_CHECK_RET(QueryNamespace(FACTORY_TABLE, nameSpace, items));
    NONZERO_CHECK_RET(QueryNamespace(USER_TABLE, nameSpace, items));
    return 0;
}

int32_t ConfigStore::Backup(const std::string& backupPath)
{
    if (mpDb == nullptr) {
        return -1;
    }
    return mpDb->BackupTo(backupPath) ? 0 : -1;
}
