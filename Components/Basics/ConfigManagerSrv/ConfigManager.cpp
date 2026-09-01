/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ConfigManager.cpp
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
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string.h>
#include "SprLog.h"
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
#include "CommonTypeDefs.h"
#include "GeneralUtils.h"
#include "ConfigManager.h"

using namespace InternalDefs;

#define LOG_TAG "ConfigM"

std::atomic<bool> ConfigManager::mRunning(true);

static const char* CONFIG_DB_PATH = "/tmp/config_manager.db";
static const char* CONFIG_BACKUP_PATH = "/tmp/config_manager.db.bak";
static const char* CONFIG_CFG_PATH = "config_manager.conf";

static std::string Trim(const std::string& text)
{
    std::string::size_type begin = text.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return "";
    }

    std::string::size_type end = text.find_last_not_of(" \t\r\n");
    return text.substr(begin, end - begin + 1);
}

static bool ParseSeedValue(const std::string& valueText,
                           std::string& nameSpace,
                           std::string& key,
                           std::string& value)
{
    std::vector<std::string> tokens = GeneralUtils::Split(valueText, '|');
    if (tokens.size() < 3) {
        return false;
    }

    nameSpace = Trim(tokens[0]);
    key = Trim(tokens[1]);
    value = tokens[2];
    for (size_t i = 3; i < tokens.size(); ++i) {
        value += "|";
        value += tokens[i];
    }

    value = Trim(value);
    return !nameSpace.empty() && !key.empty();
}

ConfigManager::ConfigManager()
    : mMutex(), mCache(), mStore(nullptr), mCfgPath(CONFIG_CFG_PATH), mDbPath(CONFIG_DB_PATH), mBackupPath(CONFIG_BACKUP_PATH), mSeedItems()
{
}

ConfigManager::~ConfigManager()
{
}

int32_t ConfigManager::Initialize()
{
    NONZERO_CHECK_RET(LoadCfgFile());

    mStore.reset(new (std::nothrow) ConfigStore(mDbPath));
    POINTER_CHECK_ERR(mStore, -1);
    NONZERO_CHECK_RET(mStore->Initialize());
    NONZERO_CHECK_RET(ApplySeedItems());
    return 0;
}

int32_t ConfigManager::LoadCfgFile()
{
    std::ifstream cfgFile(mCfgPath);
    if (!cfgFile.is_open()) {
        SPR_LOGW("Open %s failed, use default path settings\n", mCfgPath.c_str());
        return 0;
    }

    mSeedItems.clear();
    std::string line;
    while (std::getline(cfgFile, line)) {
        size_t pos = line.find('#');
        if (pos != std::string::npos) {
            line = line.substr(0, pos);
        }

        line = Trim(line);
        if (line.empty()) {
            continue;
        }

        std::string::size_type delimiter = line.find('=');
        if (delimiter == std::string::npos) {
            continue;
        }

        std::string key = Trim(line.substr(0, delimiter));
        std::string value = Trim(line.substr(delimiter + 1));
        if (key.empty()) {
            continue;
        }

        if (key == "config.db_path") {
            mDbPath = value.empty() ? CONFIG_DB_PATH : value;
            continue;
        }

        if (key == "config.backup_path") {
            mBackupPath = value.empty() ? CONFIG_BACKUP_PATH : value;
            continue;
        }

        int32_t scope = -1;
        if (key == "config.seed.default") {
            scope = CONFIG_SCOPE_DEFAULT;
        } else if (key == "config.seed.factory") {
            scope = CONFIG_SCOPE_FACTORY;
        } else if (key == "config.seed.user") {
            scope = CONFIG_SCOPE_USER;
        }

        if (scope >= CONFIG_SCOPE_DEFAULT && scope < CONFIG_SCOPE_BUTT) {
            ConfigSeedItem item;
            item.scope = scope;
            if (ParseSeedValue(value, item.nameSpace, item.key, item.value)) {
                mSeedItems.push_back(item);
            } else {
                SPR_LOGW("Ignore invalid seed config: %s\n", line.c_str());
            }
        }
    }

    return 0;
}

int32_t ConfigManager::ApplySeedItems()
{
    POINTER_CHECK_ERR(mStore, -1);
    for (size_t i = 0; i < mSeedItems.size(); ++i) {
        int32_t revision = 0;
        int32_t ret = mStore->EnsureValue(mSeedItems[i].scope,
                                          mSeedItems[i].nameSpace,
                                          mSeedItems[i].key,
                                          mSeedItems[i].value,
                                          "cfg_seed",
                                          revision);
        if (ret != 0) {
            SPR_LOGE("Seed item failed, scope=%d ns=%s key=%s\n",
                     mSeedItems[i].scope,
                     mSeedItems[i].nameSpace.c_str(),
                     mSeedItems[i].key.c_str());
            return -1;
        }
    }

    return 0;
}

int32_t ConfigManager::MainLoop()
{
    mRunning = true;
    while (mRunning) {
        usleep(100 * 1000);
    }

    return 0;
}

int32_t ConfigManager::StopWork()
{
    mRunning = false;
    return 0;
}

std::string ConfigManager::MakeCacheKey(const std::string& nameSpace, const std::string& key) const
{
    return nameSpace + "\n" + key;
}

bool ConfigManager::IsValidScope(int32_t scope) const
{
    return scope >= CONFIG_SCOPE_DEFAULT && scope < CONFIG_SCOPE_BUTT;
}

int32_t ConfigManager::SetValue(const std::string& nameSpace, const std::string& key,
                                const std::string& value, int32_t scope, int32_t& revision)
{
    if (nameSpace.empty() || key.empty() || !IsValidScope(scope)) {
        return -1;
    }

    std::lock_guard<std::mutex> lock(mMutex);
    POINTER_CHECK_ERR(mStore, -1);
    int32_t ret = mStore->Upsert(scope, nameSpace, key, value, "binder", revision);
    if (ret == 0) {
        mCache.erase(MakeCacheKey(nameSpace, key));
    }

    return ret;
}

int32_t ConfigManager::GetValue(const std::string& nameSpace, const std::string& key,
                                std::string& value, int32_t& scope, int32_t& revision)
{
    if (nameSpace.empty() || key.empty()) {
        return -1;
    }

    std::lock_guard<std::mutex> lock(mMutex);
    POINTER_CHECK_ERR(mStore, -1);
    std::string cacheKey = MakeCacheKey(nameSpace, key);
    auto it = mCache.find(cacheKey);
    if (it != mCache.end()) {
        value = it->second.value;
        scope = it->second.scope;
        revision = it->second.revision;
        return 0;
    }

    ConfigRecord record;
    int32_t ret = mStore->GetEffective(nameSpace, key, record);
    if (ret != 0) {
        return ret;
    }

    value = record.value;
    scope = record.scope;
    revision = record.revision;

    CacheEntry entry;
    entry.value = value;
    entry.scope = scope;
    entry.revision = revision;
    mCache[cacheKey] = entry;
    return 0;
}

int32_t ConfigManager::SetRawValue(const std::string& nameSpace, const std::string& key,
                                   const void* data, int32_t size, int32_t valueType,
                                   int32_t scope, int32_t& revision)
{
    if (nameSpace.empty() || key.empty() || !IsValidScope(scope) || data == nullptr || size <= 0) {
        return -1;
    }

    if (valueType <= CONFIG_VALUE_TYPE_STRING || valueType >= CONFIG_VALUE_TYPE_BUTT) {
        return -1;
    }

    const uint8_t* begin = reinterpret_cast<const uint8_t*>(data);
    std::vector<uint8_t> rawValue(begin, begin + size);

    std::lock_guard<std::mutex> lock(mMutex);
    POINTER_CHECK_ERR(mStore, -1);
    int32_t ret = mStore->UpsertRaw(scope, nameSpace, key, rawValue, valueType, "binder", revision);
    if (ret == 0) {
        mCache.erase(MakeCacheKey(nameSpace, key));
    }

    return ret;
}

int32_t ConfigManager::GetRawValue(const std::string& nameSpace, const std::string& key,
                                   void* data, int32_t size, int32_t valueType,
                                   int32_t& scope, int32_t& revision)
{
    if (nameSpace.empty() || key.empty() || data == nullptr || size <= 0) {
        return -1;
    }

    if (valueType <= CONFIG_VALUE_TYPE_STRING || valueType >= CONFIG_VALUE_TYPE_BUTT) {
        return -1;
    }

    std::lock_guard<std::mutex> lock(mMutex);
    POINTER_CHECK_ERR(mStore, -1);
    ConfigRecord record;
    int32_t ret = mStore->GetEffectiveRaw(nameSpace, key, record);
    if (ret != 0) {
        return ret;
    }

    if (record.valueType != valueType) {
        return -1;
    }

    if (static_cast<int32_t>(record.rawValue.size()) != size) {
        return -1;
    }

    memcpy(data, record.rawValue.data(), size);
    scope = record.scope;
    revision = record.revision;
    return 0;
}

int32_t ConfigManager::DeleteValue(const std::string& nameSpace, const std::string& key,
                                   int32_t scope, int32_t& revision)
{
    if (nameSpace.empty() || key.empty() || !IsValidScope(scope)) {
        return -1;
    }

    std::lock_guard<std::mutex> lock(mMutex);
    POINTER_CHECK_ERR(mStore, -1);
    int32_t ret = mStore->Remove(scope, nameSpace, key, revision);
    if (ret == 0) {
        mCache.erase(MakeCacheKey(nameSpace, key));
    }

    return ret;
}

int32_t ConfigManager::ListNamespace(const std::string& nameSpace, std::map<std::string, std::string>& items)
{
    if (nameSpace.empty()) {
        return -1;
    }

    std::lock_guard<std::mutex> lock(mMutex);
    POINTER_CHECK_ERR(mStore, -1);
    return mStore->ListNamespaceEffective(nameSpace, items);
}

int32_t ConfigManager::GetMeta(const std::string& nameSpace, const std::string& key, int32_t& scope, int32_t& revision)
{
    std::string value;
    return GetValue(nameSpace, key, value, scope, revision);
}

int32_t ConfigManager::Backup()
{
    std::lock_guard<std::mutex> lock(mMutex);
    POINTER_CHECK_ERR(mStore, -1);
    return mStore->Backup(mBackupPath);
}