/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ServiceManager.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : BSPR_log: https://linuxtaoist.gitee.io
 *  @date       : 2024/03/26
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/26 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <map>
#include <thread>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "CommonMacros.h"
#include "ServiceManager.h"

using namespace std;

#define SPR_LOGD(fmt, args...) printf("%-4d ServiceM D: " fmt, __LINE__, ##args)
#define SPR_LOGI(fmt, args...) printf("%-4d ServiceM I: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%-4d ServiceM W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%-4d ServiceM E: " fmt, __LINE__, ##args)

#define CONFIG_SUPPORT_SIGCHLD 0 // SIGCHLD不可靠。 1: 信号中断触发 0: 轮询

const char PROC_PATH[] = "/proc";
const char INIT_CONFIGURE_PATH[] = "init.conf";

ServiceManager::ServiceManager() : mRunning(false)
{

}

ServiceManager::~ServiceManager()
{
    StopWork();
}

int32_t ServiceManager::Init()
{
    return StartWork();
}

bool ServiceManager::IsExeAliveByProc(int32_t pid)
{
    struct stat fileStat;
    char pidPath[20] = {0};

    snprintf(pidPath, sizeof(pidPath), "%s/%d", PROC_PATH, pid);
    int ret = lstat(pidPath, &fileStat);
    if (ret) {
        SPR_LOGD("%s lstat failed. (%s)\n", pidPath, strerror(errno));
        return false;
    }

    // /proc/pid/ 为目录则当前进程正常
    if (S_ISDIR(fileStat.st_mode)) {
        return true;
    }

    return false;
}

int32_t ServiceManager::StartWork()
{
    if (mThread.joinable()) {
        SPR_LOGW("thread is running!");
        return 0;
    }

    StartAllExesFromConfigure(INIT_CONFIGURE_PATH);

    mRunning = true;
    mThread = thread([&]() {
        while(mRunning) {
            int pid = 0, status = 0;
            while( (pid = waitpid(-1, &status, WNOHANG)) > 0) {
                if (!IsExeAliveByProc(pid) && mPidMap.count(pid)) {
                    StartExe(mPidMap[pid].first.c_str());
                }
            }

            sleep(1);
        }
    });

    return 0;
}

int32_t ServiceManager::StopWork()
{
    if (mThread.joinable()) {
        mRunning = false;
        mThread.join();
    }

    return 0;
}

int32_t ServiceManager::StartAllExesFromConfigure(const std::string& cfgPath)
{
    int32_t startedCount = 0;
    std::ifstream configFile(cfgPath);

    if (!configFile.is_open())
    {
        SPR_LOGE("Open %s failed! (%s)\n", cfgPath.c_str(), strerror(errno));
        return -1;
    }

    bool isDependent = false;      // It is dependent on other exes
    std::string line;
    std::string lastExeName;
    while (std::getline(configFile, line))
    {
        line = line.substr(line.find_first_not_of(" \t"));
        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (line.back() == '\n') {
            line.pop_back();
        }

        size_t pos = line.find("[d]");
        if (pos != std::string::npos) {
            isDependent = true;
            line = line.substr(0, pos - 1);
        }

        // Each exe dependent by other exes is marked with [d] in the startup script.
        // After the dependent exe completes the business, it creates a node that indicates
        // the completion of the initialization business.
        // When a node is monitored, the next exe will be started. Timeout 10 * 50ms.
        if (isDependent) {
            WaitLastExeFinished(lastExeName);
        }

        ClearExeEnvNode(line);
        if (StartExe(line) == 0)
        {
            startedCount++;
            lastExeName = line;
        }
        else
        {
            SPR_LOGE("Failed to start exe: %s", line.c_str());
        }
    }

    configFile.close();
    return startedCount;
}

int32_t ServiceManager::StartExe(const std::string& exePath)
{
    static int pid = -1;

    pid = fork();
    if (pid == -1) {
        SPR_LOGE("fork failed. errno = %d (%s).\n", errno, strerror(errno));
    } else if (pid == 0) {          // child
        static int startCount = 0;

        SPR_LOGD("Pull up %s (%d).\n", exePath.c_str(), ++startCount);
        execl(exePath.c_str(), exePath.c_str(), nullptr);
    } else {                        // parent
        SPR_LOGD("Child fork pid: %d.\n", pid);

        auto it = mPidMap.begin();
        for (; it != mPidMap.end(); ++it) {
            if (exePath == it->second.first) {
                SPR_LOGW("find %s %s.\n", it->second.first.c_str(), exePath.c_str());
                break;
            }
        }

        if (it == mPidMap.end()) {
            mPidMap.insert((make_pair)(pid, make_pair(exePath, 1)));
        } else {
            it->second.second++;
            mPidMap.insert(make_pair(pid, it->second));
            mPidMap.erase(it);
        }
    }

    return 0;
}

int32_t ServiceManager::ClearExeEnvNode(const std::string& exeName)
{
    std::string monitorNode = std::string("/tmp/") + exeName;
    if (exeName.empty()) {
        return 0;
    }

    if (access(monitorNode.c_str(), F_OK) == 0) {
        unlink(monitorNode.c_str());
    }

    return 0;
}

int32_t ServiceManager::WaitLastExeFinished(const std::string& exeName)
{
    int retryTimes = 10;
    std::string monitorNode = std::string("/tmp/") + exeName;

    if (exeName.empty()) {
        return 0;
    }

    while (retryTimes--) {
        if (access(monitorNode.c_str(), F_OK) != 0) {
            SPR_LOGD("Waiting exe: %s, retryTimes = %d\n", exeName.c_str(), 10 - retryTimes);
            usleep(50000);
        } else {
            break;
        }
    }

    return 0;
}

int32_t ServiceManager::DumpPidMapInfo()
{
    SPR_LOGD("PID     PATH                TIMES+\n");
    SPR_LOGD("----------------------------------------------\n");
    for (const auto& pair : mPidMap) {
        SPR_LOGD("%-6d  %-20s %-2d \n", pair.first, pair.second.first.c_str(), pair.second.second);
    }
    SPR_LOGD("----------------------------------------------\n");

    return 0;
}

