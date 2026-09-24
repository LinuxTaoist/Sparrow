/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ServiceConfiger.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Service configuration data and loader interface.
 *  @date       : 2026/09/12
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __SERVICE_CONFIGER_H__
#define __SERVICE_CONFIGER_H__

#include <string>
#include <vector>
#include <stdint.h>

struct ServiceInfo {
    bool dependency;
    bool heartbeat;
    std::string name;
    std::string executable;
    std::string path;
    std::vector<std::string> arguments;

    ServiceInfo() : dependency(false), heartbeat(false) {}
};

using ServiceTable = std::vector<ServiceInfo>;

class ServiceConfiger {
public:
    ServiceConfiger() = default;
    ~ServiceConfiger() = default;

    ServiceConfiger(const ServiceConfiger&) = default;
    ServiceConfiger& operator=(const ServiceConfiger&) = default;

    int32_t Load(const std::string& configPath);
    int32_t GetServices(ServiceTable& services);

private:
    bool SplitTokens(const std::string& line, std::vector<std::string>& tokens);
    bool ParseServiceLine(const std::string& sourceLine, ServiceInfo& service);
    std::string Trim(const std::string& value);
    std::string RemoveComment(const std::string& line);
    std::string GetServiceName(const std::string& executable);

private:
    ServiceTable mServices;
};

#endif // __SERVICE_CONFIGER_H__
