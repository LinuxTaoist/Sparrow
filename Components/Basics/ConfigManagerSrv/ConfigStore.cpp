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
#include <stdio.h>
#include <unistd.h>
#include <sqlite3.h>
#include <vector>
#include "SprLog.h"
#include "CommonMacros.h"
#include "GeneralUtils.h"
#include "CommonTypeDefs.h"
#include "CoreTypeDefs.h"
#include "ConfigStore.h"

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

int32_t SqlExec(sqlite3* db, const std::string& sql)
{
    char* errMsg = nullptr;
    int32_t rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        SPR_LOGE("SQL exec failed: %s, err=%s\n", sql.c_str(), errMsg ? errMsg : "unknown");
        sqlite3_free(errMsg);
        return -1;
    }

    return 0;
} // namespace

static bool HasColumn(sqlite3* db, const char* tableName, const char* columnName)
{
    std::string sql = std::string("PRAGMA table_info(") + tableName + ");";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        if (name != nullptr && std::string(reinterpret_cast<const char*>(name)) == columnName) {
            found = true;
            break;
        }
    }

    sqlite3_finalize(stmt);
    return found;
}

static int32_t EnsureItemTypeColumn(sqlite3* db, const char* tableName)
{
    if (HasColumn(db, tableName, "item_type")) {
        return 0;
    }

    std::string sql = std::string("ALTER TABLE ") + tableName +
                      " ADD COLUMN item_type INTEGER NOT NULL DEFAULT 0;";
    return SqlExec(db, sql);
}

ConfigStore::ConfigStore(const std::string& dbPath)
    : mpDb(nullptr), mDbPath(dbPath), mNextRevision(0)
{
}

ConfigStore::~ConfigStore()
{
    if (mpDb != nullptr) {
        sqlite3_close(reinterpret_cast<sqlite3*>(mpDb));
        mpDb = nullptr;
    }
}

int32_t ConfigStore::Initialize()
{
    sqlite3* db = nullptr;
    if (sqlite3_open(mDbPath.c_str(), &db) != SQLITE_OK) {
        SPR_LOGE("Open db failed: %s\n", mDbPath.c_str());
        if (db != nullptr) {
            sqlite3_close(db);
        }
        return -1;
    }

    mpDb = db;
    NONZERO_CHECK_RET(SqlExec(db, "PRAGMA journal_mode=WAL;"));
    NONZERO_CHECK_RET(SqlExec(db, "PRAGMA synchronous=NORMAL;"));
    NONZERO_CHECK_RET(CreateTables());
    NONZERO_CHECK_RET(LoadNextRevision());
    return 0;
}

int32_t ConfigStore::CreateTables()
{
    sqlite3* db = reinterpret_cast<sqlite3*>(mpDb);
    POINTER_CHECK_ERR(db, -1);

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

    NONZERO_CHECK_RET(SqlExec(db, metaSql));
    char* defaultSql = sqlite3_mprintf(itemSql.c_str(), DEFAULT_TABLE);
    char* factorySql = sqlite3_mprintf(itemSql.c_str(), FACTORY_TABLE);
    char* userSql = sqlite3_mprintf(itemSql.c_str(), USER_TABLE);
    int32_t ret = 0;
    ret = SqlExec(db, defaultSql);
    if (ret == 0) {
        ret = SqlExec(db, factorySql);
    }
    if (ret == 0) {
        ret = SqlExec(db, userSql);
    }
    sqlite3_free(defaultSql);
    sqlite3_free(factorySql);
    sqlite3_free(userSql);
    if (ret != 0) {
        return -1;
    }

    NONZERO_CHECK_RET(EnsureItemTypeColumn(db, DEFAULT_TABLE));
    NONZERO_CHECK_RET(EnsureItemTypeColumn(db, FACTORY_TABLE));
    NONZERO_CHECK_RET(EnsureItemTypeColumn(db, USER_TABLE));

    return 0;
}

int32_t ConfigStore::QueryMaxRevision(const std::string& tableName, int32_t& maxRevision)
{
    sqlite3* db = reinterpret_cast<sqlite3*>(mpDb);
    POINTER_CHECK_ERR(db, -1);

    maxRevision = 0;
    std::string sql = "SELECT IFNULL(MAX(revision), 0) FROM " + tableName + ";";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return -1;
    }

    int32_t ret = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        maxRevision = sqlite3_column_int(stmt, 0);
        ret = 0;
    }
    sqlite3_finalize(stmt);
    return ret;
}

int32_t ConfigStore::LoadNextRevision()
{
    sqlite3* db = reinterpret_cast<sqlite3*>(mpDb);
    POINTER_CHECK_ERR(db, -1);

    sqlite3_stmt* stmt = nullptr;
    std::string sql = std::string("SELECT meta_value FROM ") + META_TABLE +
                      " WHERE meta_key='next_revision';";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return -1;
    }

    int32_t ret = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* text = sqlite3_column_text(stmt, 0);
        mNextRevision = text ? atoi(reinterpret_cast<const char*>(text)) : 1;
        ret = 0;
    }
    sqlite3_finalize(stmt);

    if (ret == 0 && mNextRevision > 0) {
        return 0;
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
    sqlite3* db = reinterpret_cast<sqlite3*>(mpDb);
    POINTER_CHECK_ERR(db, -1);
    std::string sql = std::string(
        "INSERT OR REPLACE INTO ") + META_TABLE +
        "(meta_key, meta_value) VALUES('next_revision', ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return -1;
    }

    std::string revisionText = std::to_string(revision);
    sqlite3_bind_text(stmt, 1, revisionText.c_str(), -1, SQLITE_TRANSIENT);
    int32_t ret = (sqlite3_step(stmt) == SQLITE_DONE) ? 0 : -1;
    sqlite3_finalize(stmt);
    return ret;
}

int32_t ConfigStore::BeginTransaction()
{
    return SqlExec(reinterpret_cast<sqlite3*>(mpDb), "BEGIN IMMEDIATE TRANSACTION;");
}

int32_t ConfigStore::CommitTransaction()
{
    return SqlExec(reinterpret_cast<sqlite3*>(mpDb), "COMMIT;");
}

int32_t ConfigStore::RollbackTransaction()
{
    return SqlExec(reinterpret_cast<sqlite3*>(mpDb), "ROLLBACK;");
}

const char* ConfigStore::TableName(int32_t scope) const
{
    switch (scope) {
        case CONFIG_SCOPE_DEFAULT:
            return DEFAULT_TABLE;
        case CONFIG_SCOPE_FACTORY:
            return FACTORY_TABLE;
        case CONFIG_SCOPE_USER:
            return USER_TABLE;
        default:
            return nullptr;
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
    sqlite3* db = reinterpret_cast<sqlite3*>(mpDb);
    POINTER_CHECK_ERR(db, -1);
    const char* tableName = TableName(scope);
    if (tableName == nullptr) {
        return -1;
    }

    NONZERO_CHECK_RET(BeginTransaction());
    revision = mNextRevision;
    mNextRevision++;

    std::string sql = std::string(
        "INSERT OR REPLACE INTO ") + tableName +
        "(namespace_name, item_key, item_value, item_type, revision, schema_version, owner, update_time) "
        "VALUES(?, ?, ?, ?, ?, 1, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        RollbackTransaction();
        return -1;
    }

    std::string now = GetNowString();
    sqlite3_bind_text(stmt, 1, nameSpace.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 3,
                      value.empty() ? nullptr : reinterpret_cast<const void*>(value.data()),
                      static_cast<int>(value.size()), SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, valueType);
    sqlite3_bind_int(stmt, 5, revision);
    sqlite3_bind_text(stmt, 6, owner.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, now.c_str(), -1, SQLITE_TRANSIENT);

    int32_t ret = (sqlite3_step(stmt) == SQLITE_DONE) ? 0 : -1;
    sqlite3_finalize(stmt);
    if (ret != 0 || UpdateNextRevision(mNextRevision) != 0 || CommitTransaction() != 0) {
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
    sqlite3* db = reinterpret_cast<sqlite3*>(mpDb);
    POINTER_CHECK_ERR(db, -1);
    const char* tableName = TableName(scope);
    if (tableName == nullptr) {
        return -1;
    }

    NONZERO_CHECK_RET(BeginTransaction());
    revision = mNextRevision;
    mNextRevision++;

    std::string sql = std::string("DELETE FROM ") + tableName + " WHERE namespace_name=? AND item_key=?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        RollbackTransaction();
        return -1;
    }

    sqlite3_bind_text(stmt, 1, nameSpace.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, key.c_str(), -1, SQLITE_TRANSIENT);
    int32_t ret = (sqlite3_step(stmt) == SQLITE_DONE) ? 0 : -1;
    sqlite3_finalize(stmt);
    if (ret != 0 || UpdateNextRevision(mNextRevision) != 0 || CommitTransaction() != 0) {
        RollbackTransaction();
        mNextRevision = revision;
        return -1;
    }

    return 0;
}

int32_t ConfigStore::QueryTable(const std::string& tableName, const std::string& nameSpace,
                                const std::string& key, ConfigRecord& record)
{
    sqlite3* db = reinterpret_cast<sqlite3*>(mpDb);
    POINTER_CHECK_ERR(db, -1);

    std::string sql = "SELECT item_value, item_type, revision, owner, update_time FROM " + tableName
                    + " WHERE namespace_name=? AND item_key=?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return -1;
    }

    sqlite3_bind_text(stmt, 1, nameSpace.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, key.c_str(), -1, SQLITE_TRANSIENT);

    int32_t ret = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const void* value = sqlite3_column_blob(stmt, 0);
        int valueBytes = sqlite3_column_bytes(stmt, 0);
        const unsigned char* owner = sqlite3_column_text(stmt, 3);
        const unsigned char* updateTime = sqlite3_column_text(stmt, 4);

        record.rawValue.clear();
        if (value != nullptr && valueBytes > 0) {
            const uint8_t* begin = reinterpret_cast<const uint8_t*>(value);
            record.rawValue.assign(begin, begin + valueBytes);
        }

        record.valueType = sqlite3_column_int(stmt, 1);
        if (record.valueType == CONFIG_VALUE_TYPE_STRING) {
            record.value.assign(record.rawValue.begin(), record.rawValue.end());
        } else {
            record.value.clear();
        }

        record.revision = sqlite3_column_int(stmt, 2);
        record.owner = owner ? reinterpret_cast<const char*>(owner) : "";
        record.updateTime = updateTime ? reinterpret_cast<const char*>(updateTime) : "";
        ret = 0;
    }

    sqlite3_finalize(stmt);
    return ret;
}

int32_t ConfigStore::GetByScope(int32_t scope, const std::string& nameSpace, const std::string& key, ConfigRecord& record)
{
    const char* tableName = TableName(scope);
    if (tableName == nullptr) {
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
    sqlite3* db = reinterpret_cast<sqlite3*>(mpDb);
    POINTER_CHECK_ERR(db, -1);
    std::string sql = "SELECT item_key, item_value, item_type FROM " + tableName + " WHERE namespace_name=?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return -1;
    }

    sqlite3_bind_text(stmt, 1, nameSpace.c_str(), -1, SQLITE_TRANSIENT);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* key = sqlite3_column_text(stmt, 0);
        const void* value = sqlite3_column_blob(stmt, 1);
        int valueBytes = sqlite3_column_bytes(stmt, 1);
        int valueType = sqlite3_column_int(stmt, 2);
        if (key != nullptr && valueType == CONFIG_VALUE_TYPE_STRING) {
            const char* valueData = reinterpret_cast<const char*>(value);
            items[reinterpret_cast<const char*>(key)] =
                (valueData != nullptr && valueBytes > 0) ? std::string(valueData, valueBytes) : "";
        }
    }

    sqlite3_finalize(stmt);
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
    sqlite3* db = reinterpret_cast<sqlite3*>(mpDb);
    POINTER_CHECK_ERR(db, -1);

    sqlite3* backupDb = nullptr;
    if (sqlite3_open(backupPath.c_str(), &backupDb) != SQLITE_OK) {
        if (backupDb != nullptr) {
            sqlite3_close(backupDb);
        }
        return -1;
    }

    sqlite3_backup* backup = sqlite3_backup_init(backupDb, "main", db, "main");
    if (backup == nullptr) {
        sqlite3_close(backupDb);
        return -1;
    }

    int32_t rc = sqlite3_backup_step(backup, -1);
    sqlite3_backup_finish(backup);
    sqlite3_close(backupDb);
    return (rc == SQLITE_DONE) ? 0 : -1;
}