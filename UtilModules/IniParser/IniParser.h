/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IniParser.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Simple INI configuration parser.
 *  @date       : 2026/09/20
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __INI_PARSER_H__
#define __INI_PARSER_H__

#include <map>
#include <string>
#include <functional>
#include <stdint.h>

class IniParser {
public:
    using Section = std::map<std::string, std::string>;
    using Sections = std::map<std::string, Section>;

    IniParser() = default;
    ~IniParser() = default;

    int32_t Load(const std::string& path);
    std::string GetValue(const std::string& section,
                         const std::string& key,
                         const std::string& defaultValue = "") const;
    Sections GetSections() const;
    Section GetSection(const std::string& section) const;
    void ForEachSection(std::function<bool(const std::string& section)> callback) const;
    void ForEachKey(const std::string& section,
                    std::function<bool(const std::string& key, const std::string& value)> callback) const;
    void ForEach(std::function<bool(const std::string& section,
                                       const std::string& key,
                                       const std::string& value)> callback) const;

private:
    std::string Trim(const std::string& value);

private:
    Sections mSections;
};

#endif // __INI_PARSER_H__
