/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ServiceManager.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : BSPR_log: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
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
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "GeneralUtils.h"
#include "CoreTypeDefs.h"
#include "ServiceManager.h"

using namespace std;
using namespace GeneralUtils;

#define SPR_LOGI(fmt, args...) printf("%s %6d %-12s I: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "SrvMgr", __LINE__, ##args)
#define SPR_LOGD(fmt, args...) printf("%s %6d %-12s D: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "SrvMgr", __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%s %6d %-12s W: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "SrvMgr", __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%s %6d %-12s E: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "SrvMgr", __LINE__, ##args)

const char PROC_PATH[] = "/proc";
const char ENV_ROOT_PATH[] = "/tmp/";
const char INIT_CONFIGURE_PATH[] = "init.conf";

bool ServiceManager::mRunning = false;

ServiceManager::ServiceManager()
{
}

ServiceManager::~ServiceManager()
{
    StopWork();
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

    // /proc/pid/ 目录存在则当前进程正常
    if (S_ISDIR(fileStat.st_mode)) {
        return true;
    }

    return false;
}

int32_t ServiceManager::StartWork()
{
    StartAllExesFromConfigure(INIT_CONFIGURE_PATH);
    mRunning = true;
    while(mRunning) {
        int pid = 0, status = 0;
        while( (pid = waitpid(-1, &status, WNOHANG)) > 0) {
            if (!IsExeAliveByProc(pid) && mPidMap.count(pid)) {
                StartExe(mPidMap[pid].first.c_str());
            }
        }

        sleep(1);
    }

    StopAllSubExes();
    SPR_LOGI("Service manager loop exit!\n");
    return 0;
}

int32_t ServiceManager::StopAllSubExes()
{
    // 子进程退出顺序与启动顺序相反
    for (auto rit = mPidMap.rbegin(); rit != mPidMap.rend(); ++rit) {
        kill(rit->first, MAIN_EXIT_SIGNUM);
        SPR_LOGI("KILL %d TO %d: %s\n", MAIN_EXIT_SIGNUM, rit->first, rit->second.first.c_str());

        int status = 0;
        waitpid(rit->first, &status, 0);
        SPR_LOGI("EXIT PID: %d, STATUS: %d, PATH: %s\n", rit->first, status, rit->second.first.c_str());
    }

    return 0;
}

int32_t ServiceManager::StopWork()
{
    mRunning = false;
    SPR_LOGI("Stop work!\n");
    return 0;
}

int32_t ServiceManager::StartAllExesFromConfigure(const std::string& cfgPath)
{
    int32_t startedCount = 0;
    std::ifstream configFile(cfgPath);

    if (!configFile.is_open()) {
        SPR_LOGE("Open %s failed! (%s)\n", cfgPath.c_str(), strerror(errno));
        return -1;
    }

    std::string line;
    std::string lastExeName;
    while (std::getline(configFile, line)) {
        line = line.substr(line.find_first_not_of(" \t"));
        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (line.back() == '\n') {
            line.pop_back();
        }

        bool isDependent = false;
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
        if (StartExe(line) == 0) {
            startedCount++;
            lastExeName = line;
        } else {
            SPR_LOGE("Failed to start exe: %s", line.c_str());
        }
    }

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

        for (int fd = sysconf(_SC_OPEN_MAX); fd > 2; fd--) {
            close(fd);
        }

        SPR_LOGD("execl %s (%d).\n", exePath.c_str(), ++startCount);
        execl(exePath.c_str(), exePath.c_str(), nullptr);
    } else {                        // parent
        std::string srvName = GeneralUtils::GetSubstringAfterLastDelimiter(exePath, '/').c_str();
        if (srvName.empty()) {
            srvName = exePath;
        }

        SPR_LOGD("service: %-20s pid: %6d\n", srvName.c_str(), pid);

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
    if (exeName.empty()) {
        return 0;
    }

    std::string monitorNode = std::string(ENV_ROOT_PATH) + exeName;
    if (access(monitorNode.c_str(), F_OK) == 0) {
        unlink(monitorNode.c_str());
    }

    return 0;
}

int32_t ServiceManager::WaitLastExeFinished(const std::string& exeName)
{
    if (exeName.empty()) {
        return 0;
    }

    int retryTimes = 10;
    std::string monitorNode = std::string(ENV_ROOT_PATH) + exeName;
    while (retryTimes--) {
        if (access(monitorNode.c_str(), F_OK) != 0) {
            SPR_LOGD("Waiting exe: %-20s retryTimes = %-3d\n", exeName.c_str(), 10 - retryTimes);
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
        SPR_LOGD("%6d  %-20s %2d \n", pair.first, pair.second.first.c_str(), pair.second.second);
    }
    SPR_LOGD("----------------------------------------------\n");

    return 0;
}

