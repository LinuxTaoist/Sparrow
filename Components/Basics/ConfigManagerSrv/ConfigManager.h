/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ConfigManager.h
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
#ifndef __CONFIG_MANAGER_H__
#define __CONFIG_MANAGER_H__

#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <vector>
#include <string>
#include <stdint.h>
#include "ConfigStore.h"

class ConfigManager
{
public:
    ConfigManager();
    ~ConfigManager();

    int32_t Initialize();
    int32_t MainLoop();
    static int32_t StopWork();

    int32_t SetValue(const std::string& nameSpace, const std::string& key,
                     const std::string& value, int32_t scope, int32_t& revision);
    int32_t GetValue(const std::string& nameSpace, const std::string& key,
                     std::string& value, int32_t& scope, int32_t& revision);
    int32_t DeleteValue(const std::string& nameSpace, const std::string& key,
                        int32_t scope, int32_t& revision);
    int32_t ListNamespace(const std::string& nameSpace, std::map<std::string, std::string>& items);
    int32_t GetMeta(const std::string& nameSpace, const std::string& key, int32_t& scope, int32_t& revision);
    int32_t Backup();

    int32_t SetRawValue(const std::string& nameSpace, const std::string& key,
                        const void* data, int32_t size, int32_t valueType,
                        int32_t scope, int32_t& revision);
    int32_t GetRawValue(const std::string& nameSpace, const std::string& key,
                        void* data, int32_t size, int32_t valueType,
                        int32_t& scope, int32_t& revision);

private:
    struct ConfigSeedItem {
        int32_t scope;
        std::string nameSpace;
        std::string key;
        std::string value;
    };

    int32_t LoadCfgFile();
    int32_t ApplySeedItems();
    struct CacheEntry {
        std::string value;
        int32_t scope;
        int32_t revision;
    };

    std::string MakeCacheKey(const std::string& nameSpace, const std::string& key) const;
    bool IsValidScope(int32_t scope) const;

private:
    static std::atomic<bool> mRunning;
    std::mutex mMutex;
    std::map<std::string, CacheEntry> mCache;
    std::unique_ptr<ConfigStore> mStore;
    std::string mCfgPath;
    std::string mDbPath;
    std::string mBackupPath;
    std::vector<ConfigSeedItem> mSeedItems;
};

#endif // __CONFIG_MANAGER_H__