/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprSystem.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/02/24
 *
 *  System initialization file
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/02/24 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <memory>
#include <fstream>
#include <fcntl.h>
#include <dlfcn.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/resource.h>
#include "SprLog.h"
#include "SprContext.h"
#include "CommonMacros.h"
#include "SprSystem.h"
#include "SprTimeTrace.h"
#include "CoreTypeDefs.h"
#include "SprSystemTimer.h"
#include "SprTimerManager.h"
#include "EpollEventHandler.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("SprSystem", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("SprSystem", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("SprSystem", fmt, ##args)

#define TTP(ID, TEXT) SprTimeTrace::GetInstance()->TimeTracePoint(ID, TEXT)
#define LOCAL_PATH_VERSION  "/tmp/sparrow_version"

SprSystem::SprSystem()
{
    mInotifyFd = -1;
}

SprSystem::~SprSystem()
{
    ReleasePlugins();
}

SprSystem* SprSystem::GetInstance()
{
    static SprSystem instance;
    return &instance;
}

void SprSystem::InitEnv()
{
    // Init msg queue limit
    InitMsgQueueLimit();

    // write release information
    LoadReleaseInformation();
}

void SprSystem::InitWatchDir()
{
    mInotifyFd = mDirWatch.GetInotifyFd();
    std::string path = DEFAULT_PLUGIN_LIBRARY_PATH;
    if (access(DEFAULT_PLUGIN_LIBRARY_PATH, F_OK) == -1) {
        GetDefaultLibraryPath(path);
        SPR_LOGW("%s not exist, changed path %s\n", DEFAULT_PLUGIN_LIBRARY_PATH, path.c_str());
    }

    mDirWatch.AddDirWatch(path.c_str());
    mFilePtr = std::make_shared<PFile>(mInotifyFd, [&](int fd, void *arg) {
        const int size = 100;
        char buffer[size];
        ssize_t numRead = read(fd, buffer, size);
        if (numRead == -1) {
            SPR_LOGE("read %d failed! (%s)\n", fd, strerror(errno));
            return;
        }

        int offset = 0;
        while (offset < numRead) {
            struct inotify_event* pEvent = reinterpret_cast<struct inotify_event*>(&buffer[offset]);
            if (!pEvent) {
                SPR_LOGE("pEvent is nullptr!\n");
                return;
            }

            if (pEvent->len > 0) {
                if (pEvent->mask & IN_CREATE) {
                    SPR_LOGD("File %s is created\n", pEvent->name);
                }
                if (pEvent->mask & IN_DELETE) {
                    SPR_LOGD("File %s is deleted\n", pEvent->name);
                }
            }
            offset += sizeof(struct inotify_event) + pEvent->len;
        }
    });

    EpollEventHandler::GetInstance()->AddPoll(mFilePtr.get());
}

void SprSystem::InitMsgQueueLimit()
{
    // The limit for creating message queues has to be changed, otherwise the other
    // applications can not create enough message queues.
    // Note: The values in /proc/sys/fs/mqueue/* seem to have no influence on this issue.
    // Also ulimit -n has no influence on this issue.
    struct rlimit rlim = {RLIM_INFINITY, RLIM_INFINITY};
    int ret = getrlimit(RLIMIT_MSGQUEUE, &rlim);
    if (ret == 0)
    {
        rlim.rlim_cur = RLIM_INFINITY;  // soft limit
        rlim.rlim_max = RLIM_INFINITY;  // hard limit
        setrlimit(RLIMIT_MSGQUEUE, &rlim);
    }
}

void SprSystem::LoadReleaseInformation()
{
    std::string projectInfo     = PROJECT_INFO;
    std::string cxxStandard     = CXX_STANDARD;
    std::string gxxStandard     = GXX_VERSION;
    std::string gccVersion      = GCC_VERSION;
    std::string runEnv          = RUN_ENV;
    std::string buildTime       = BUILD_TIME;
    std::string buildType       = BUILD_TYPE;
    std::string buildHost       = BUILD_HOST;
    std::string buildPlatform   = BUILD_PLATFORM;
    std::string moduleConfig    = MODULE_CONFIG_VERSION;

    std::string releaseInfo;
    releaseInfo += "System Version : " + projectInfo + "\n";
    releaseInfo += "C++ Standard   : " + cxxStandard + "\n";
    releaseInfo += "G++ Version    : " + gxxStandard + "\n";
    releaseInfo += "Gcc Version    : " + gccVersion + "\n";
    releaseInfo += "Running Env    : " + runEnv + "\n";
    releaseInfo += "Build Time     : " + buildTime + "\n";
    releaseInfo += "Build Type     : " + buildType + "\n";
    releaseInfo += "Build Host     : " + buildHost + "\n";
    releaseInfo += "Build Platform : " + buildPlatform + "\n";
    releaseInfo += "Module Config  : " + moduleConfig + "\n";

    std::ofstream file(LOCAL_PATH_VERSION);
    if (file) {
        SPR_LOGD("Load system version information to %s\n", LOCAL_PATH_VERSION);
        file << releaseInfo;
        file.close();
    } else {
        SPR_LOGE("Open %s fail!\n", LOCAL_PATH_VERSION);
    }
}

void SprSystem::GetDefaultLibraryPath(std::string& path)
{
    char curPath[100] = {};
    if (getcwd(curPath, sizeof(curPath)) == nullptr) {
        SPR_LOGE("Get current path fail! (%s)\n", strerror(errno));
        return;
    }

    path = curPath + std::string("/../Lib");
}

void SprSystem::LoadPlugins()
{
    std::string path = DEFAULT_PLUGIN_LIBRARY_PATH;
    if (access(DEFAULT_PLUGIN_LIBRARY_PATH, F_OK) == -1) {
        GetDefaultLibraryPath(path);
        SPR_LOGW("%s not exist, changed path %s\n", DEFAULT_PLUGIN_LIBRARY_PATH, path.c_str());
    }

    DIR* dir = opendir(path.c_str());
    if (dir == nullptr) {
        SPR_LOGE("Open %s fail! (%s)\n", path, strerror(errno));
        return;
    }

    // loop: find all plugins library files in path
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, DEFAULT_PLUGIN_LIBRARY_FILE_PREFIX, strlen(DEFAULT_PLUGIN_LIBRARY_FILE_PREFIX)) != 0) {
            continue;
        }

        void* pDlHandler = dlopen(entry->d_name, RTLD_NOW);
        if (!pDlHandler) {
            SPR_LOGE("Load plugin %s fail! (%s)\n", entry->d_name, dlerror() ? dlerror() : "unknown error");
            continue;
        }

        auto pEntry = (PluginEntryFunc)dlsym(pDlHandler, DEFAULT_PLUGIN_LIBRARY_ENTRY_FUNC);
        if (!pEntry) {
            SPR_LOGE("Find %s fail in %s! (%s)\n", DEFAULT_PLUGIN_LIBRARY_ENTRY_FUNC, entry->d_name, dlerror() ? dlerror() : "unknown error");
            dlclose(pDlHandler);
            continue;
        }

        mPluginHandles.push_back(pDlHandler);
        mPluginEntries.push_back(pEntry);
        SPR_LOGD("Load plugin %s success!\n", entry->d_name);
    }

    closedir(dir);
}

void SprSystem::ReleasePlugins()
{
    for (auto& mPluginHandle : mPluginHandles) {
        dlclose(mPluginHandle);
    }

    mPluginHandles.clear();
}

int SprSystem::EnvReady(const std::string& srvName)
{
    std::string node = "/tmp/" + srvName;
    int fd = creat(node.c_str(), 0644);
    if (fd != -1) {
        close(fd);
    }

    return 0;
}

void SprSystem::Init()
{
    SPR_LOGD("=============================================\n");
    SPR_LOGD("=          Sprrow System Init               =\n");
    SPR_LOGD("=============================================\n");

    InitEnv();
    LoadPlugins();  // load plugin libraries
    InitWatchDir();

    TTP(9, "systemTimerPtr->Initialize()");
    shared_ptr<SprSystemTimer> systemTimerPtr = make_shared<SprSystemTimer>(MODULE_SYSTEM_TIMER, "SysTimer");
    systemTimerPtr->Initialize();

    TTP(10, "TimerManager->Initialize()");
    SprTimerManager::GetInstance(MODULE_TIMERM, "TimerM", systemTimerPtr)->Initialize();

    // excute plugin entry function
    SprContext ctx;
    for (auto& mPluginEntry : mPluginEntries) {
        mPluginEntry(mModules, ctx);
    }

    // excute plug module initialize function
    for (auto& module : mModules) {
        module.second->Initialize();
    }

    EnvReady(SRV_NAME_SPARROW);
}
