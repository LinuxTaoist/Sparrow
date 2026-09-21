/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IniParser.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Simple INI configuration parser.
 *  @date       : 2026/09/20
 *
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <fstream>
#include <cctype>
#include <utility>
#include "IniParser.h"

std::string IniParser::Trim(const std::string& value) {
    size_t begin = 0;
    while (begin < value.size() && std::isspace(static_cast<unsigned char>(value[begin]))) {
        ++begin;
    }

    size_t end = value.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(begin, end - begin);
}

int32_t IniParser::Load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return -1;
    }

    Sections sections;
    std::string currentSection;
    std::string line;
    while (std::getline(file, line)) {
        line = Trim(line);
        size_t comment = line.find_first_of("#;");
        if (comment != std::string::npos) {
            line = Trim(line.substr(0, comment));
        }
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        if (line.front() == '[' && line.back() == ']') {
            currentSection = Trim(line.substr(1, line.size() - 2));
            if (currentSection.empty()) {
                continue;
            }
            sections[currentSection];
            continue;
        }

        size_t delimiter = line.find('=');
        if (delimiter == std::string::npos || currentSection.empty()) {
            continue;
        }

        std::string key = Trim(line.substr(0, delimiter));
        std::string value = Trim(line.substr(delimiter + 1));
        if (!key.empty()) {
            sections[currentSection][key] = value;
        }
    }

    mSections = std::move(sections);
    return 0;
}

std::string IniParser::GetValue(const std::string& section,
                                const std::string& key,
                                const std::string& defaultValue) const {
    auto sectionIt = mSections.find(section);
    if (sectionIt == mSections.end()) {
        return defaultValue;
    }

    auto valueIt = sectionIt->second.find(key);
    return (valueIt != sectionIt->second.end()) ? valueIt->second : defaultValue;
}

IniParser::Sections IniParser::GetSections() const {
    return mSections;
}

IniParser::Section IniParser::GetSection(const std::string& section) const {
    auto sectionIt = mSections.find(section);
    return (sectionIt != mSections.end()) ? sectionIt->second : Section();
}

void IniParser::ForEachSection(std::function<bool(const std::string& section)> callback) const {
    if (!callback) {
        return;
    }

    for (const auto& section : mSections) {
        if (!callback(section.first)) {
            break;
        }
    }
}

void IniParser::ForEachKey(const std::string& section,
        std::function<bool(const std::string& key, const std::string& value)> callback) const {
    if (!callback) {
        return;
    }

    auto sectionIt = mSections.find(section);
    if (sectionIt == mSections.end()) {
        return;
    }

    for (const auto& keyValue : sectionIt->second) {
        if (!callback(keyValue.first, keyValue.second)) {
            break;
        }
    }
}

void IniParser::ForEach(
    std::function<bool(const std::string& section,
                       const std::string& key,
                       const std::string& value)> callback) const {
    if (!callback) {
        return;
    }

    for (const auto& section : mSections) {
        for (const auto& keyValue : section.second) {
            if (!callback(section.first, keyValue.first, keyValue.second)) {
                return;
            }
        }
    }
}
