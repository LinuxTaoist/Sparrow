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
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
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

#define SRV_GRACEFUL_STOP_POLL_CNT       50
#define SRV_RESTART_DELAY_US             500000   // 500ms delay before restart to avoid flooding
#define SRV_DEPENDENCY_START_GAP_US      100000   // 100ms
#define INIT_CONFIGURE_FILE             "init.conf"

bool ServiceManager::mRunning = false;

ServiceManager::ServiceManager() {
}

ServiceManager::~ServiceManager() {
    mHeartbeatMonitor.Stop();
    ExitLoop();
}

int32_t ServiceManager::Init() {
    int32_t ret = InitEnv();
    if (ret < 0) {
        SPR_LOGE("Init env failed!\n");
        return -1;
    }

    ret = LoadSrvsInfo();
    if (ret != 0) {
        SPR_LOGE("Get service config failed!\n");
        return -1;
    }

    ret = InitHeartbeatMonitor();
    if (ret != 0) {
        SPR_LOGE("Init heartbeat monitor failed!\n");
        return -1;
    }

    ret = StartAllExes();
    if (ret < 0) {
        SPR_LOGE("Start services failed!\n");
        return -1;
    }

    return 0;
}

int32_t ServiceManager::InitEnv() {
    struct rlimit rlim;
    if (getrlimit(RLIMIT_MSGQUEUE, &rlim) != 0) {
        return 0;
    }

    if (rlim.rlim_cur == RLIM_INFINITY &&
        rlim.rlim_max == RLIM_INFINITY) {
        return 0;
    }

    rlim.rlim_cur = RLIM_INFINITY;
    rlim.rlim_max = RLIM_INFINITY;
    if (setrlimit(RLIMIT_MSGQUEUE, &rlim) != 0) {
        SPR_LOGW("setrlimit failed! (%s)\n", strerror(errno));
    }

    return 0;
}

int32_t ServiceManager::LoadSrvsInfo() {
    std::string path = GetInitCfgPath();
    if (path.empty()) {
        SPR_LOGE("Get path failed!\n");
        return -1;
    }

    int32_t ret = mSrvConfiger.Load(path);
    if (ret != 0) {
        SPR_LOGE("Load service config %s failed!\n", path.c_str());
        return -1;
    }

    ServiceTable services;
    ret = mSrvConfiger.GetServices(services);
    if (ret != 0) {
        SPR_LOGE("Get service config %s failed!\n", path.c_str());
        return -1;
    }

    mSrvs.clear();
    mSrvs.reserve(services.size());
    for (const ServiceInfo& service : services) {
        mSrvs.emplace_back(service);
    }

    return 0;
}

int32_t ServiceManager::InitHeartbeatMonitor() {
    std::vector<std::string> heartbeatServices;
    for (const SrvInfo& service : mSrvs) {
        if (service.heartbeat) {
            heartbeatServices.push_back(service.name);
        }
    }

    if (heartbeatServices.empty()) {
        SPR_LOGI("Not exist heartbeat services\n");
        return 0;
    }

    int32_t ret = mHeartbeatMonitor.Start(
        HEARTBEAT_DEFAULT_CHANNEL_PATH,
        HEARTBEAT_DEFAULT_TIMEOUT_MS,
        heartbeatServices,
        [this](const std::string& serviceName, bool alive) {
            if (!alive) {
                SPR_LOGW("Exe %s heartbeat timeout!\n", serviceName.c_str());
                MarkSrvAbnormal(serviceName);
            }
        });
    if (ret != 0) {
        SPR_LOGE("Monitor heartbeat failed!\n");
    }

    return ret;
}

int32_t ServiceManager::StartAllExes() {
    bool waitPrev = false;
    for (SrvInfo& service : mSrvs) {
        if (service.dependency && waitPrev) {
            usleep(SRV_DEPENDENCY_START_GAP_US);
        }

        int32_t ret = StartOne(service);
        if (ret == 0) {
            waitPrev = service.dependency;
        } else {
            SPR_LOGE("Start %s failed!\n", service.path.c_str());
        }
    }

    SPR_LOGI("Started %zu services\n", mSrvs.size());
    return (int32_t)mSrvs.size();
}

int32_t ServiceManager::ForkExec(const std::string& exePath,
                                 const std::vector<std::string>& arguments) {
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

        std::vector<char*> argv;
        argv.reserve(arguments.size() + 2);
        argv.push_back(const_cast<char*>(exePath.c_str()));
        for (const std::string& argument : arguments) {
            argv.push_back(const_cast<char*>(argument.c_str()));
        }

        argv.push_back(nullptr);
        execv(exePath.c_str(), argv.data());
        SPR_LOGE("execv %s failed (%s)\n", exePath.c_str(), strerror(errno));
        _exit(127);
    }

    return pid;
}

int32_t ServiceManager::IsValidExecFile(const std::string& exePath) {
    if (exePath.empty()) {
        SPR_LOGE("exePath is empty!\n");
        return -1;
    }

    struct stat st = {};
    if (stat(exePath.c_str(), &st) != 0) {
        SPR_LOGE("File %s does not exist! (%s)\n", exePath.c_str(), strerror(errno));
        return -1;
    }

    if (!S_ISREG(st.st_mode)) {
        SPR_LOGE("File %s is not a regular file!\n", exePath.c_str());
        return -1;
    }

    if (access(exePath.c_str(), X_OK) != 0) {
        SPR_LOGE("File %s is not executable!\n", exePath.c_str());
        return -1;
    }

    return 0;
}

int32_t ServiceManager::StartOne(SrvInfo& service) {
    int32_t ret = IsValidExecFile(service.path);
    if (ret < 0) {
        SPR_LOGE("Exec %s is invalid!\n", service.path.c_str());
        return -1;
    }

    int32_t pid = ForkExec(service.path, service.arguments);
    if (pid == -1) {
        return -1;
    }

    {
        std::lock_guard<std::mutex> lock(mSrvMutex);
        service.pid = pid;
        service.abnormal = false;
    }
    SPR_LOGD("service: %-20s pid: %6d\n", service.name.c_str(), pid);
    return 0;
}

int32_t ServiceManager::StopOne(SrvInfo& service) {
    if (service.pid <= 0) {
        SPR_LOGI("%s already dead\n", service.path.c_str());
        return 0;
    }

    if (kill(service.pid, 0) == -1 && errno == ESRCH) {
        SPR_LOGI("%s (pid %d) already gone\n", service.path.c_str(), service.pid);
        service.pid = -1;
        return 0;
    }

    SPR_LOGI("Stopping %s (pid %d)\n", service.path.c_str(), service.pid);
    kill(service.pid, MAIN_EXIT_SIGNUM);

    int32_t status = 0;
    pid_t ret = 0;
    int32_t elapsed = 0;
    while (elapsed < SRV_GRACEFUL_STOP_POLL_CNT) {
        ret = waitpid(service.pid, &status, WNOHANG);
        if (ret > 0 || (ret < 0 && errno == ECHILD)) {
            break;
        }
        usleep(100000);
        elapsed++;
    }

    if (ret <= 0 && !(ret < 0 && errno == ECHILD)) {
        SPR_LOGW("%s didn't exit, sending SIGKILL, ret = %d\n", service.path.c_str(), ret);
        kill(service.pid, SIGKILL);
        waitpid(service.pid, &status, 0);
    }

    service.pid = -1;
    SPR_LOGI("%s exited, status %d\n", service.path.c_str(), status);
    return 0;
}

int32_t ServiceManager::StopAll() {
    mHeartbeatMonitor.Stop();

    int32_t status = 0;
    while (waitpid(-1, &status, WNOHANG) > 0);

    for (auto it = mSrvs.rbegin(); it != mSrvs.rend(); ++it) {
        StopOne(*it);
    }

    mSrvs.clear();
    return 0;
}

int32_t ServiceManager::TryRestart(size_t idx) {
    SrvInfo service;
    {
        std::lock_guard<std::mutex> lock(mSrvMutex);
        if (idx >= mSrvs.size() || !mSrvs[idx].abnormal) {
            return 0;
        }

        service = mSrvs[idx];
    }

    usleep(SRV_RESTART_DELAY_US);
    if (service.pid > 0) {
        SPR_LOGW("Restarting unhealthy %s (pid %d)\n", service.name.c_str(), service.pid);
        StopOne(service);
    }
    if (service.heartbeat) {
        mHeartbeatMonitor.Reset(service.name);
    }

    SPR_LOGI("Restarting %s (attempt %d)\n", service.path.c_str(), service.restartCount + 1);
    int32_t pid = ForkExec(service.path, service.arguments);
    if (pid == -1) {
        SPR_LOGE("ForkExec %s failed\n", service.path.c_str());
        return -1;
    }

    {
        std::lock_guard<std::mutex> lock(mSrvMutex);
        if (idx < mSrvs.size()) {
            mSrvs[idx].pid = pid;
            mSrvs[idx].abnormal = false;
            mSrvs[idx].restartCount++;
        }
    }
    SPR_LOGD("service: %-20s pid: %6d [cnt: %d]\n",
             service.name.c_str(), pid, service.restartCount + 1);
    return 0;
}

void ServiceManager::MarkSrvAbnormal(const std::string& serviceName) {
    std::lock_guard<std::mutex> lock(mSrvMutex);
    for (SrvInfo& srv : mSrvs) {
        if (srv.name == serviceName) {
            srv.abnormal = true;
            return;
        }
    }
}

std::string ServiceManager::GetInitCfgPath() {
    const char* pEnvRoot = std::getenv(ENV_SPR_ROOT_PATH);
    if (pEnvRoot != nullptr && pEnvRoot[0] != '\0') {
        return std::string(pEnvRoot) + "/" + DEFAULT_SPR_ETC_FILE + "/" + INIT_CONFIGURE_FILE;
    }

    char exePath[300] = {0};
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len > 0) {
        exePath[len] = '\0';
        std::string fullPath(exePath);
        std::string::size_type pos = fullPath.find_last_of('/');
        if (pos != std::string::npos) {
            std::string execDir = fullPath.substr(0, pos);
            return execDir + "/../" + DEFAULT_SPR_ETC_FILE + "/" + INIT_CONFIGURE_FILE;
        }
    }

    return "";
}

int32_t ServiceManager::WorkLoop() {
    mRunning = true;
    while (mRunning) {
        // reap newly exited children
        int32_t pid = 0, status = 0;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            std::lock_guard<std::mutex> lock(mSrvMutex);
            for (size_t i = 0; i < mSrvs.size(); i++) {
                if (mSrvs[i].pid == pid) {
                    SPR_LOGW("%s (pid %d) exited, status %d\n", mSrvs[i].path.c_str(), pid, status);
                    mSrvs[i].pid = -1;
                    mSrvs[i].abnormal = true;
                    break;
                }
            }
        }

        for (size_t i = 0; i < mSrvs.size(); ++i) {
            TryRestart(i);
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

int32_t ServiceManager::DumpPidMapInfo() {
    SPR_LOGD("PID     PATH                RESTARTS\n");
    SPR_LOGD("-----------------------------------------\n");
    for (const auto& srv : mSrvs) {
        if (srv.pid > 0) {
            SPR_LOGD("%6d  %-20s %2d\n", srv.pid, srv.path.c_str(), srv.restartCount);
        } else {
            SPR_LOGD("  DEAD  %-20s %2d\n", srv.path.c_str(), srv.restartCount);
        }
    }
    SPR_LOGD("-----------------------------------------\n");
    return 0;
}
