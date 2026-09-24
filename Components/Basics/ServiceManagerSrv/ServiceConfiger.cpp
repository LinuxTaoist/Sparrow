/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ServiceConfiger.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Service configuration file parser implementation.
 *  @date       : 2026/09/12
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <set>
#include <fstream>
#include "ServiceConfiger.h"

bool ServiceConfiger::SplitTokens(const std::string& line,
                                  std::vector<std::string>& tokens) {
    bool quoted = false;
    bool escaped = false;
    bool bracketed = false;
    std::string token;

    for (size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];
        if (escaped) {
            token.push_back(ch);
            escaped = false;
            continue;
        }
        if (ch == '\\' && quoted) {
            escaped = true;
            continue;
        }
        if (ch == '"') {
            quoted = !quoted;
            continue;
        }
        if (ch == '[' && !quoted && token.empty()) {
            bracketed = true;
        }
        if (ch == ']' && bracketed && !quoted) {
            bracketed = false;
        }
        if ((ch == ' ' || ch == '\t') && !quoted && !bracketed) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            continue;
        }
        token.push_back(ch);
    }

    if (quoted || escaped || bracketed) {
        return false;
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return true;
}

bool ServiceConfiger::ParseServiceLine(const std::string& sourceLine,
                                       ServiceInfo& service) {
    std::string line = Trim(RemoveComment(sourceLine));
    if (line.empty()) {
        return true;
    }

    // Ignore UTF-8 BOM if it appears at the beginning of the first path.
    if (line.size() >= 3 &&
        static_cast<unsigned char>(line[0]) == 0xEF &&
        static_cast<unsigned char>(line[1]) == 0xBB &&
        static_cast<unsigned char>(line[2]) == 0xBF) {
        line.erase(0, 3);
        line = Trim(line);
    }

    std::vector<std::string> tokens;
    if (!SplitTokens(line, tokens) || tokens.empty()) {
        return false;
    }

    // E.g.:
    // logmanagersrv [d] [argv: --heartbeat=on --config=/etc/sparrow.conf]
    // bindermanagersrv [d]
    service = ServiceInfo();
    service.executable = tokens[0];
    service.path = tokens[0];
    service.name = GetServiceName(service.executable);
    if (service.name.empty()) {
        return false;
    }

    for (size_t i = 1; i < tokens.size(); ++i) {
        const std::string& tag = tokens[i];
        if (tag.size() < 3 || tag.front() != '[' || tag.back() != ']') {
            return false;
        }

        const std::string value = tag.substr(1, tag.size() - 2);
        if (value == "d") {
            service.dependency = true;
        } else if (value.compare(0, 5, "argv:") == 0) {
            const std::string argumentText = Trim(value.substr(5));
            std::vector<std::string> arguments;
            if (argumentText.empty() || !SplitTokens(argumentText, arguments)) {
                return false;
            }
            for (const std::string& argument : arguments) {
                service.arguments.push_back(argument);
                if (argument == "--heartbeat=on") {
                    service.heartbeat = true;
                } else if (argument == "--heartbeat=off") {
                    service.heartbeat = false;
                }
            }
        } else {
            // Unknown tags are rejected deliberately. This prevents a typo from
            // silently changing the service configuration.
            return false;
        }
    }

    return true;
}

std::string ServiceConfiger::Trim(const std::string& value) {
    const std::string whitespace = " \t\r\n";
    const size_t begin = value.find_first_not_of(whitespace);
    if (begin == std::string::npos) {
        return std::string();
    }

    const size_t end = value.find_last_not_of(whitespace);
    return value.substr(begin, end - begin + 1);
}

std::string ServiceConfiger::RemoveComment(const std::string& line) {
    bool quoted = false;
    bool escaped = false;

    for (size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];
        if (escaped) {
            escaped = false;
            continue;
        }
        if (ch == '\\' && quoted) {
            escaped = true;
            continue;
        }
        if (ch == '"') {
            quoted = !quoted;
            continue;
        }
        if (ch == '#' && !quoted) {
            return line.substr(0, i);
        }
    }

    return line;
}

std::string ServiceConfiger::GetServiceName(const std::string& executable) {
    const size_t end = executable.find_last_not_of('/');
    if (end == std::string::npos) {
        return std::string();
    }

    const size_t begin = executable.find_last_of('/', end);
    return executable.substr(begin == std::string::npos ? 0 : begin + 1,
                             end - (begin == std::string::npos ? 0 : begin + 1) + 1);
}

int32_t ServiceConfiger::Load(const std::string& configPath) {
    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
        return -1;
    }

    std::string line;
    ServiceTable services;
    std::set<std::string> serviceNames;

    while (std::getline(configFile, line)) {
        ServiceInfo service;
        if (!ParseServiceLine(line, service)) {
            return -1;
        }
        if (service.executable.empty()) {
            continue;
        }
        if (!serviceNames.insert(service.name).second) {
            return -1;
        }
        services.push_back(service);
    }

    if (configFile.bad()) {
        return -1;
    }

    mServices.swap(services);
    return 0;
}

int32_t ServiceConfiger::GetServices(ServiceTable& services) {
    services = mServices;
    return 0;
}
