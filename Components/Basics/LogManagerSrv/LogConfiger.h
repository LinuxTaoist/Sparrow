/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : LogConfiger.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Log configuration loader.
 *  @date       : 2026/09/20
 *
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __LOG_CONFIGER_H__
#define __LOG_CONFIGER_H__

#include <map>
#include <string>
#include <stdint.h>

class LogConfiger {
public:
    using LogModuleAttrs = std::map<std::string, std::string>;
    using LogModules = std::map<std::string, LogModuleAttrs>;

    LogConfiger();
    ~LogConfiger() = default;

    int32_t Load(const std::string& path);
    int32_t GetLogModules(LogModules& modules) const;

private:
    int32_t InitModules(LogModules& modules) const;
    int32_t UpdateModuleAttr(const std::string& moduleName,
                             const std::string& key,
                             const std::string& value,
                             LogModules& modules) const;

private:
    LogModules mModules;
};

#endif // __LOG_CONFIGER_H__
