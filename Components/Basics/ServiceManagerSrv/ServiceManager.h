/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ServiceManager.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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

#include <map>
#include <string>
#include <thread>
#include <stdint.h>

class ServiceManager
{
public:
    ServiceManager();
    ~ServiceManager();

    int32_t Init();
    int32_t DumpPidMapInfo();

private:
    bool IsExeAliveByProc(int32_t pid);

    int32_t StartWork();
    int32_t StopWork();
    int32_t StartAllExesFromConfigure(const std::string& cfgPath);
    int32_t StartExe(const std::string& exePath);
    int32_t ClearExeEnvNode(const std::string& exeName);
    int32_t WaitLastExeFinished(const std::string& exeName);

private:
    bool mRunning;
    std::thread mThread;
    std::map<int, std::pair<std::string, int>> mPidMap;     // key: pid, value: exePath, times
};

#endif // __SERVICE_MANAGER_H__
