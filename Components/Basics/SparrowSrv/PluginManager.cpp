/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PluginManager.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/10/18
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/10/18 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <dlfcn.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "SprLog.h"
#include "SprDebugNode.h"
#include "CoreTypeDefs.h"
#include "PluginManager.h"

using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("PlugMgr", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("PlugMgr", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("PlugMgr", fmt, ##args)

#define DEFAULT_HOT_PLUG_ENABLE true
#define OWNER_PLUGINMGR         "PluginManager"

PluginManager::PluginManager() : mHotPlugEnable(DEFAULT_HOT_PLUG_ENABLE)
{
}

PluginManager::~PluginManager()
{
    UnregisterDebugFuncs();
    UnloadAllPlugins();
}

void PluginManager::Init()
{
    mDefaultLibPath = GetDefaultLibraryPath();
    LoadAllPlugins();
    InitWatchDir();
    RegisterDebugFuncs();
}

void PluginManager::InitWatchDir()
{
    // Add a watch on the specified directory. The events to monitor include:
    // - IN_CLOSE_WRITE: Triggered when a file is closed after being written.
    // - IN_DELETE: Triggered when a file or directory is deleted.
    // - IN_MOVED_TO: Triggered when a file or directory is moved to the specified directory.
    // - IN_MOVED_FROM: Triggered when a file or directory is moved from the specified directory.
    // Note: IN_CREATE is not used because it triggers immediately when a file is created,
    // which may result in attempting to process the file before it is fully written and closed.
    mDirWatch.AddDirWatch(mDefaultLibPath.c_str(), IN_CLOSE_WRITE | IN_MOVED_TO | IN_MOVED_FROM | IN_DELETE);
    mpFile = std::make_shared<PFile>(mDirWatch.GetInotifyFd(), [&](int fd, void *arg) {
        const int size = 100;
        char buffer[size] = {0};
        ssize_t numRead = read(fd, buffer, size);
        if (numRead == -1) {
            SPR_LOGE("read %d failed! (%s)\n", fd, strerror(errno));
            return;
        }

        PluginManager* pMySelf = reinterpret_cast<PluginManager*>(arg);
        if (pMySelf && !pMySelf->mHotPlugEnable) {
            SPR_LOGD("Hot plug disabled, ignore event\n");
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
                if (pEvent->mask & IN_CLOSE_WRITE || pEvent->mask & IN_MOVED_TO) {
                    SPR_LOGD("File %s is created\n", pEvent->name);
                    LoadPlugin(pEvent->name);
                }
                if (pEvent->mask & IN_DELETE || pEvent->mask & IN_MOVED_FROM) {
                    SPR_LOGD("File %s is deleted\n", pEvent->name);
                    UnloadPlugin(pEvent->name);
                }
            }
            offset += sizeof(struct inotify_event) + pEvent->len;
        }
    }, this);

    mpFile->AddToPoll();
}

std::string PluginManager::GetDefaultLibraryPath()
{
    std::string path = DEFAULT_PLUGIN_LIBRARY_PATH;
    if (access(DEFAULT_PLUGIN_LIBRARY_PATH, F_OK) == -1) {
        SPR_LOGW("%s not exist, changed path %s\n", DEFAULT_PLUGIN_LIBRARY_PATH, path.c_str());
        char curPath[100] = {};
        if (getcwd(curPath, sizeof(curPath)) == nullptr) {
            SPR_LOGE("Get current path fail! (%s)\n", strerror(errno));
            return "";
        }
        path = curPath + std::string("/../Lib");
    }

    return path;
}

void PluginManager::LoadPlugin(const std::string& path)
{
    if (strncmp(path.c_str(), DEFAULT_PLUGIN_LIBRARY_FILE_PREFIX, strlen(DEFAULT_PLUGIN_LIBRARY_FILE_PREFIX)) != 0) {
        return;
    }

    if (mPluginHandles.find(path) != mPluginHandles.end()) {
        SPR_LOGW("Plugin %s already loaded!\n", path.c_str());
        return;
    }

    void* pDlHandler = dlopen(path.c_str(), RTLD_NOW);
    if (!pDlHandler) {
        SPR_LOGE("Load plugin %s fail! (%s)\n", path.c_str(), dlerror());
        return;
    }

    auto pEntry = (PluginEntryFunc)dlsym(pDlHandler, DEFAULT_PLUGIN_LIBRARY_ENTRY_FUNC);
    if (!pEntry) {
        SPR_LOGE("Find %s fail in %s! (%s)\n", DEFAULT_PLUGIN_LIBRARY_ENTRY_FUNC, path.c_str(), dlerror());
        dlclose(pDlHandler);
        return;
    }

    pEntry(mPluginModules, mContext);
    mPluginHandles[path] = pDlHandler;
    SPR_LOGD("Load plugin %s success!\n", path.c_str());
}

void PluginManager::UnloadPlugin(const std::string& path) {
    if (strncmp(path.c_str(), DEFAULT_PLUGIN_LIBRARY_FILE_PREFIX, strlen(DEFAULT_PLUGIN_LIBRARY_FILE_PREFIX)) != 0) {
        return;
    }

    if (mPluginHandles.find(path) == mPluginHandles.end()) {
        SPR_LOGW("Plugin %s not loaded!\n", path.c_str());
        return;
    }

    void* pDlHandler = mPluginHandles[path];
    if (!pDlHandler) {
        SPR_LOGE("Load plugin %s fail! (%s)\n", path.c_str(), dlerror());
        return;
    }

    auto pExit = (PluginExitFunc)dlsym(pDlHandler, DEFAULT_PLUGIN_LIBRARY_EXIT_FUNC);
    if (!pExit) {
        SPR_LOGE("Find %s fail in %s! (%s)\n", DEFAULT_PLUGIN_LIBRARY_EXIT_FUNC, path.c_str(), dlerror());
        dlclose(pDlHandler);
        return;
    }

    pExit(mPluginModules, mContext);
    dlclose(pDlHandler);
    int num = mPluginHandles.erase(path);
    SPR_LOGD("Unload plugin %s %s!\n", path.c_str(), num > 0 ? "success" : "fail");
}

void PluginManager::LoadAllPlugins()
{
    DIR* dir = opendir(mDefaultLibPath.c_str());
    if (dir == nullptr) {
        SPR_LOGE("Open %s fail! (%s)\n", mDefaultLibPath.c_str(), strerror(errno));
        return;
    }

    // loop: find all plugins library files in path
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        LoadPlugin(entry->d_name);
    }

    closedir(dir);
}

void PluginManager::UnloadAllPlugins()
{
    for (auto& handle : mPluginHandles) {
        UnloadPlugin(handle.first);
        handle.second = nullptr;
    }
    mPluginHandles.clear();
}

void PluginManager::RegisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    p->RegisterCmd(OWNER_PLUGINMGR, "DumpPluginMgr",  "Dump Info",          std::bind(&PluginManager::DebugDumpPlugMInfo,  this, std::placeholders::_1));
    p->RegisterCmd(OWNER_PLUGINMGR, "EnableHotPlug",  "Enable hot plug",    std::bind(&PluginManager::DebugEnableHotPlug,  this, std::placeholders::_1));
    p->RegisterCmd(OWNER_PLUGINMGR, "DisableHotPlug", "Disable hot plug",   std::bind(&PluginManager::DebugDisableHotPlug, this, std::placeholders::_1));
}

void PluginManager::UnregisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    p->UnregisterCmd(OWNER_PLUGINMGR);
}

void PluginManager::DebugDumpPlugMInfo(const std::string& args)
{
    SPR_LOGD("-------------- Dump PlugManager Info --------------\n");
    SPR_LOGD("- mHotPlugEnable = %d\n", mHotPlugEnable);
    SPR_LOGD("---------------------------------------------------\n");
}

void PluginManager::DebugEnableHotPlug(const std::string& args)
{
    mHotPlugEnable = true;
}

void PluginManager::DebugDisableHotPlug(const std::string& args)
{
    mHotPlugEnable = false;
}
