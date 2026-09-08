/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ConfigStore.h
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
#ifndef __CONFIG_STORE_H__
#define __CONFIG_STORE_H__

#include <map>
#include <string>
#include <vector>
#include <stdint.h>
#include "SqliteAdapter.h"

struct ConfigRecord
{
    std::string value;
    std::vector<uint8_t> rawValue;
    int32_t valueType;
    int32_t revision;
    int32_t scope;
    std::string owner;
    std::string updateTime;
};

class ConfigStore
{
public:
    explicit ConfigStore(const std::string& dbPath);
    ~ConfigStore();

    int32_t Initialize();
    int32_t Upsert(int32_t scope, const std::string& nameSpace, const std::string& key,
                   const std::string& value, const std::string& owner, int32_t& revision);
    int32_t UpsertRaw(int32_t scope, const std::string& nameSpace, const std::string& key,
                      const std::vector<uint8_t>& value, int32_t valueType,
                      const std::string& owner, int32_t& revision);
    int32_t EnsureValue(int32_t scope, const std::string& nameSpace, const std::string& key,
                        const std::string& value, const std::string& owner, int32_t& revision);
    int32_t Remove(int32_t scope, const std::string& nameSpace, const std::string& key, int32_t& revision);
    int32_t GetByScope(int32_t scope, const std::string& nameSpace, const std::string& key, ConfigRecord& record);
    int32_t GetByScopeRaw(int32_t scope, const std::string& nameSpace, const std::string& key, ConfigRecord& record);
    int32_t GetEffective(const std::string& nameSpace, const std::string& key, ConfigRecord& record);
    int32_t GetEffectiveRaw(const std::string& nameSpace, const std::string& key, ConfigRecord& record);
    int32_t ListNamespaceEffective(const std::string& nameSpace, std::map<std::string, std::string>& items);
    int32_t Backup(const std::string& backupPath);

private:
    int32_t CreateTables();
    int32_t LoadNextRevision();
    int32_t UpdateNextRevision(int32_t revision);
    int32_t BeginTransaction();
    int32_t CommitTransaction();
    int32_t RollbackTransaction();
    int32_t QueryMaxRevision(const std::string& tableName, int32_t& maxRevision);
    int32_t QueryTable(const std::string& tableName, const std::string& nameSpace,
                       const std::string& key, ConfigRecord& record);
    int32_t QueryNamespace(const std::string& tableName, const std::string& nameSpace,
                           std::map<std::string, std::string>& items);
    std::string TableName(int32_t scope) const;

private:
    SqliteAdapter* mpDb;
    std::string mDbPath;
    int32_t mNextRevision;
};

#endif // __CONFIG_STORE_H__