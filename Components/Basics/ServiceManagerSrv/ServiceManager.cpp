/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ServiceManager.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
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
#include <fstream>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include "GeneralUtils.h"
#include "CoreTypeDefs.h"
#include "CommonMacros.h"
#include "ServiceManager.h"

using namespace std;
using namespace GeneralUtils;

#define SPR_LOGI(fmt, args...) printf("%s %6d %-12s I: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "SrvMgr", __LINE__, ##args)
#define SPR_LOGD(fmt, args...) printf("%s %6d %-12s D: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "SrvMgr", __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%s %6d %-12s W: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "SrvMgr", __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%s %6d %-12s E: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "SrvMgr", __LINE__, ##args)

#define SRV_RESTART_DELAY_US             500000   // 500ms delay before restart to avoid flooding
#define SRV_GRACEFUL_STOP_POLL_CNT       10
#define SRV_DEPENDENCY_START_GAP_US      100000   // 100ms

const char INIT_CONFIGURE_PATH[] = "init.conf";
bool ServiceManager::mRunning = false;

ServiceManager::ServiceManager() {
}

ServiceManager::~ServiceManager() {
    ExitLoop();
}

int32_t ServiceManager::InitEnv() {
    // Raise msg queue limit for child processes
    struct rlimit rlim = {RLIM_INFINITY, RLIM_INFINITY};
    int32_t ret = getrlimit(RLIMIT_MSGQUEUE, &rlim);
    if (ret == 0) {
        rlim.rlim_cur = RLIM_INFINITY;
        rlim.rlim_max = RLIM_INFINITY;
        setrlimit(RLIMIT_MSGQUEUE, &rlim);
    }
    return 0;
}

int32_t ServiceManager::WorkLoop() {
    InitEnv();
    StartAllFromConfig(INIT_CONFIGURE_PATH);
    mRunning = true;

    while (mRunning) {
        // reap newly exited children
        int32_t pid = 0, status = 0;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            for (size_t i = 0; i < mSvcs.size(); i++) {
                if (mSvcs[i].pid == pid) {
                    SPR_LOGW("%s (pid %d) exited, status %d\n", mSvcs[i].path.c_str(), pid, status);
                    mSvcs[i].pid = -1;       // mark dead, TryRestart will re-fork
                    TryRestart(i);
                    break;
                }
            }
        }

        sleep(1);
    }

    StopAll();
    SPR_LOGI("Service manager loop exit!\n");
    return 0;
}

int32_t ServiceManager::ExitLoop() {
    mRunning = false;
    SPR_LOGI("Stop work!\n");
    return 0;
}

int32_t ServiceManager::StartAllFromConfig(const std::string& cfgPath) {
    ifstream cfgFile(cfgPath);
    if (!cfgFile.is_open()) {
        SPR_LOGE("Open %s failed! (%s)\n", cfgPath.c_str(), strerror(errno));
        return -1;
    }

    std::string line;
    bool waitPrev = false;

    while (getline(cfgFile, line)) {
        size_t start = line.find_first_not_of(" \t");
        if (start == string::npos) {
            continue;
        }

        line = line.substr(start);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        if (line.back() == '\n') {
            line.pop_back();
        }

        bool dep = false;
        size_t dpos = line.find("[d]");
        if (dpos != string::npos) {
            dep = true;
            line = line.substr(0, dpos - 1);
        }

        if (dep && waitPrev) {
            usleep(SRV_DEPENDENCY_START_GAP_US);
        }

        if (StartOne(line) == 0) {
            waitPrev = dep;
        } else {
            SPR_LOGE("Failed to start: %s\n", line.c_str());
        }
    }

    SPR_LOGI("Started %zu services\n", mSvcs.size());
    return (int32_t)mSvcs.size();
}

int32_t ServiceManager::ForkExec(const std::string& exePath) {
    int32_t pid = fork();
    if (pid == -1) {
        SPR_LOGE("fork failed (%s)\n", strerror(errno));
        return -1;
    }

    if (pid == 0) {
        int32_t maxFd = sysconf(_SC_OPEN_MAX);
        if (maxFd > 1024) {
            maxFd = 1024;
        }
        for (int32_t fd = 3; fd < maxFd; fd++) {
            close(fd);
        }

        execl(exePath.c_str(), exePath.c_str(), nullptr);
        SPR_LOGE("execl %s failed (%s)\n", exePath.c_str(), strerror(errno));
        _exit(127);
    }

    return pid;
}

int32_t ServiceManager::StartOne(const std::string& exePath) {
    int32_t pid = ForkExec(exePath);
    if (pid == -1) {
        return -1;
    }

    string name = GetSubstringAfterLastDelimiter(exePath, '/');
    if (name.empty()) {
        name = exePath;
    }

    mSvcs.emplace_back(exePath, pid);
    SPR_LOGD("service: %-20s pid: %6d\n", name.c_str(), pid);
    return 0;
}

int32_t ServiceManager::StopAll() {
    // 1. Sweep residual zombies before shutdown to avoid stale-PID kills
    int32_t status = 0;
    while (waitpid(-1, &status, WNOHANG) > 0);

    for (auto it = mSvcs.rbegin(); it != mSvcs.rend(); ++it) {
        // Skip services that are already dead (marked by WorkLoop)
        if (it->pid <= 0) {
            SPR_LOGI("%s already dead\n", it->path.c_str());
            continue;
        }

        // Guard against PID recycling: skip entries whose PID already gone
        if (kill(it->pid, 0) == -1 && errno == ESRCH) {
            SPR_LOGI("%s (pid %d) already gone\n", it->path.c_str(), it->pid);
            continue;
        }

        SPR_LOGI("Stopping %s (pid %d)\n", it->path.c_str(), it->pid);
        kill(it->pid, MAIN_EXIT_SIGNUM);

        // 3. Poll-wait for graceful exit (100 ms × SRV_GRACEFUL_STOP_POLL_CNT)
        pid_t ret = 0;
        int32_t elapsed = 0;
        while (elapsed < SRV_GRACEFUL_STOP_POLL_CNT) {
            ret = waitpid(it->pid, &status, WNOHANG);
            if (ret > 0) {
                break;
            }
            usleep(100000);
            elapsed++;
        }

        // 4. Hard-kill fallback if still alive after timeout
        if (ret <= 0) {
            SPR_LOGW("%s didn't exit, sending SIGKILL\n", it->path.c_str());
            kill(it->pid, SIGKILL);
            waitpid(it->pid, &status, 0);
        }

        SPR_LOGI("%s exited, status %d\n", it->path.c_str(), status);
    }

    mSvcs.clear();
    return 0;
}

int32_t ServiceManager::TryRestart(size_t idx) {
    if (idx >= mSvcs.size()) {
        return -1;
    }

    SvcInfo& svc = mSvcs[idx];
    usleep(SRV_RESTART_DELAY_US);

    SPR_LOGI("Restarting %s (attempt %d)\n", svc.path.c_str(), svc.restartCount + 1);
    int32_t pid = ForkExec(svc.path);
    if (pid == -1) {
        SPR_LOGE("ForkExec %s failed\n", svc.path.c_str());
        return -1;
    }

    svc.pid = pid;
    svc.restartCount++;
    string name = GetSubstringAfterLastDelimiter(svc.path, '/');
    SPR_LOGD("service: %-20s pid: %6d [cnt: %d]\n", name.c_str(), pid, svc.restartCount);
    return 0;
}

int32_t ServiceManager::DumpPidMapInfo() {
    SPR_LOGD("PID     PATH                RESTARTS\n");
    SPR_LOGD("-----------------------------------------\n");
    for (const auto& svc : mSvcs) {
        if (svc.pid > 0) {
            SPR_LOGD("%6d  %-20s %2d\n", svc.pid, svc.path.c_str(), svc.restartCount);
        } else {
            SPR_LOGD("  DEAD  %-20s %2d\n", svc.path.c_str(), svc.restartCount);
        }
    }
    SPR_LOGD("-----------------------------------------\n");
    return 0;
}

