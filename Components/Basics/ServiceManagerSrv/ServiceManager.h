/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ServiceManager.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SERVICE_MANAGER_H__
#define __SERVICE_MANAGER_H__

#include <mutex>
#include <string>
#include <vector>
#include <stdint.h>
#include "ServiceConfiger.h"
#include "HeartbeatMonitor.h"

struct SrvInfo {
    bool dependency;
    bool heartbeat;
    bool abnormal;
    int32_t pid;            // > 0 running, -1 dead
    int32_t restartCount;
    std::string name;
    std::string path;
    std::vector<std::string> arguments;

    SrvInfo()
        : dependency(false), heartbeat(false), abnormal(false), pid(-1), restartCount(1) {
    }

    explicit SrvInfo(const ServiceInfo& service)
        : dependency(service.dependency),
          heartbeat(service.heartbeat),
          abnormal(false),
          pid(-1),
          restartCount(1),
          name(service.name),
          path(service.path),
          arguments(service.arguments) {
    }
};

class ServiceManager {
public:
    ServiceManager();
    ~ServiceManager();

    int32_t Init();
    int32_t WorkLoop();
    static int32_t ExitLoop();

private:
    int32_t InitEnv();
    int32_t LoadSrvsInfo();
    int32_t InitHeartbeatMonitor();
    int32_t IsValidExecFile(const std::string& exePath);
    int32_t StartAllExes();
    int32_t StartOne(SrvInfo& service);
    int32_t ForkExec(const std::string& exePath, const std::vector<std::string>& arguments);
    int32_t StopOne(SrvInfo& service);
    int32_t StopAll();
    int32_t TryRestart(size_t idx);
    void MarkSrvAbnormal(const std::string& serviceName);
    int32_t DumpPidMapInfo();
    std::string GetInitCfgPath();

private:
    static bool mRunning;
    std::mutex mSrvMutex;
    std::vector<SrvInfo> mSrvs;
    ServiceConfiger mSrvConfiger;
    HeartbeatMonitor mHeartbeatMonitor;
};

#endif // __SERVICE_MANAGER_H__
