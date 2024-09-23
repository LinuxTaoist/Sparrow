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
#include "CommonMacros.h"
#include "SprSystem.h"
#include "SprTimeTrace.h"
#include "CoreTypeDefs.h"
#include "SprSystemTimer.h"
#include "SprTimerManager.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("SprSystem", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("SprSystem", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("SprSystem", fmt, ##args)

#define TTP(ID, TEXT) SprTimeTrace::GetInstance()->TimeTracePoint(ID, TEXT)
#define LOCAL_PATH_VERSION  "/tmp/sparrow_version"

SprSystem::SprSystem()
{
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


    std::ofstream file(LOCAL_PATH_VERSION);
    if (file)
    {
        SPR_LOGD("Load system version information to %s\n", LOCAL_PATH_VERSION);
        file << releaseInfo;
        file.close();
    }
    else
    {
        SPR_LOGE("Open %s fail!\n", LOCAL_PATH_VERSION);
    }
}

void SprSystem::LoadPlugins()
{
    DIR *dir = opendir(PLUGIN_LIBRARY_PATH);
    if (dir == nullptr) {
        SPR_LOGE("Open %s fail! (%s)\n", PLUGIN_LIBRARY_PATH, strerror(errno));
        return;
    }

    // loop: find all plugins library files in PLUGIN_LIBRARY_PATH
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, PLUGIN_LIBRARY_FILE_PREFIX, strlen(PLUGIN_LIBRARY_FILE_PREFIX)) != 0) {
            continue;
        }

        void* pDlHandler = dlopen(entry->d_name, RTLD_NOW);
        if (!pDlHandler) {
            SPR_LOGE("Load plugin %s fail! (%s)\n", entry->d_name, dlerror() ? dlerror() : "unknown error");
            continue;
        }

        auto pEntry = (PluginEntryFunc)dlsym(pDlHandler, PLUGIN_LIBRARY_ENTRY_FUNC);
        if (!pEntry) {
            SPR_LOGE("Find %s fail in %s! (%s)\n", PLUGIN_LIBRARY_ENTRY_FUNC, entry->d_name, dlerror() ? dlerror() : "unknown error");
            dlclose(pDlHandler);
            continue;
        }

        mPluginHandles.push_back(pDlHandler);
        mPluginEntries.push_back(pEntry);
        SPR_LOGD("Load plugin %s success\n", entry->d_name);
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

    TTP(9, "systemTimerPtr->Initialize()");
    shared_ptr<SprSystemTimer> systemTimerPtr = make_shared<SprSystemTimer>(MODULE_SYSTEM_TIMER, "SysTimer");
    systemTimerPtr->Initialize();

    TTP(10, "TimerManager->Initialize()");
    SprTimerManager::GetInstance(MODULE_TIMERM, "TimerM", systemTimerPtr)->Initialize();

    // Init other modules
    for (auto& mPluginEntry : mPluginEntries) {
        static int pluginIndex = 0;
        std::string plugInDesc;
        mPluginEntry(pluginIndex++, plugInDesc);
    }

    EnvReady(SRV_NAME_SPARROW);
}
