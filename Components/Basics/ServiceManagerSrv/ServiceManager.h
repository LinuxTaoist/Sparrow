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

#include <string>
#include <vector>
#include <stdint.h>

struct SvcInfo {
    int32_t pid;            // > 0 running, -1 dead
    int32_t restartCount;
    std::string path;

    SvcInfo() = default;
    SvcInfo(const std::string& p, int32_t id)
        : pid(id), restartCount(1), path(p) {}
};

class ServiceManager {
public:
    ServiceManager();
    ~ServiceManager();

    int32_t WorkLoop();
    static int32_t ExitLoop();

private:
    int32_t InitEnv();
    int32_t IsValidExecFile(const std::string& exePath);
    int32_t StartAllFromConfig(const std::string& cfgPath);
    int32_t StartOne(const std::string& exePath);
    int32_t ForkExec(const std::string& exePath);
    int32_t StopAll();
    int32_t TryRestart(size_t idx);
    int32_t DumpPidMapInfo();
    std::string GetInitCfgPath();

private:
    static bool mRunning;
    std::vector<SvcInfo> mSvcs;
};

#endif // __SERVICE_MANAGER_H__
