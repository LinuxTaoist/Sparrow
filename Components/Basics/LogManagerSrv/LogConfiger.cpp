/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : LogConfiger.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Log configuration loader.
 *  @date       : 2026/09/20
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <utility>
#include "IniParser.h"
#include "LogConfiger.h"
#include "LogConfigKeys.h"

LogConfiger::LogConfiger() {
    InitModules(mModules);
}

int32_t LogConfiger::InitModules(LogModules& modules) const {
    modules.clear();
    modules[LOG_CONFIG_MODULE_DEFAULT] = {
        {LOG_CONFIG_KEY_FRAME_LENGTH_BYTES, "1024"},
        {LOG_CONFIG_KEY_ENABLED,            LOG_CONFIG_VALUE_TRUE},
        {LOG_CONFIG_KEY_LEVEL,              LOG_CONFIG_VALUE_DEBUG},
        {LOG_CONFIG_KEY_OUTPUT,             LOG_CONFIG_VALUE_FILE},
        {LOG_CONFIG_KEY_FILE_NAME,          LOG_CONFIG_DEFAULT_FILE_NAME},
        {LOG_CONFIG_KEY_FILE_NAME_FORMAT,   ""},
        {LOG_CONFIG_KEY_FILE_PATH,          LOG_CONFIG_DEFAULT_FILE_PATH},
        {LOG_CONFIG_KEY_FILE_NUM,           "10"},
        {LOG_CONFIG_KEY_FILE_CAPACITY_MB,   "10"},
        {LOG_CONFIG_KEY_FLUSH_COUNT,        "64"},
        {LOG_CONFIG_KEY_FLUSH_INTERVAL_MS,  "1000"},
    };
    return 0;
}

int32_t LogConfiger::Load(const std::string& path) {
    LogModules modules;
    int32_t ret = InitModules(modules);
    if (ret != 0) {
        return -1;
    }

    IniParser parser;
    ret = parser.Load(path);
    if (ret != 0) {
        mModules = std::move(modules);
        return -1;
    }

    parser.ForEach([this, &modules](const std::string& section,
                                    const std::string& key,
                                    const std::string& value) {
        UpdateModuleAttr(section, key, value, modules);
        return true;
    });

    const LogConfiger::LogModuleAttrs defaultAttrs = modules.at(LOG_CONFIG_MODULE_DEFAULT);
    for (auto& module : modules) {
        if (module.first == LOG_CONFIG_MODULE_DEFAULT) {
            continue;
        }

        LogConfiger::LogModuleAttrs completeAttrs = defaultAttrs;
        for (const auto& attr : module.second) {
            completeAttrs[attr.first] = attr.second;
        }
        module.second = std::move(completeAttrs);
    }

    mModules = std::move(modules);
    return 0;
}

int32_t LogConfiger::GetLogModules(LogModules& modules) const {
    modules = mModules;
    return 0;
}

int32_t LogConfiger::UpdateModuleAttr(const std::string& moduleName,
                                      const std::string& key,
                                      const std::string& value,
                                      LogModules& modules) const {
    const std::string outputPrefix = LOG_CONFIG_OUTPUT_PREFIX;
    if (moduleName.compare(0, outputPrefix.size(), outputPrefix) != 0) {
        return 0;
    }

    const std::string module = moduleName.substr(outputPrefix.size());
    if (module.empty()) {
        return -1;
    }

    auto moduleIt = modules.find(module);
    if (moduleIt == modules.end()) {
        moduleIt = modules.emplace(module, LogConfiger::LogModuleAttrs()).first;
    }

    moduleIt->second[key] = value;
    return 0;
}
