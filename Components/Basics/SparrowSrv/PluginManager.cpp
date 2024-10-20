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
#include "CoreTypeDefs.h"
#include "EpollEventHandler.h"
#include "PluginManager.h"

using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("PlugMgr", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("PlugMgr", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("PlugMgr", fmt, ##args)

PluginManager::PluginManager()
{
    mInotifyFd = -1;
}

PluginManager::~PluginManager()
{
    UnloadAllPlugins();
}

void PluginManager::Init()
{
    mDefaultLibPath = GetDefaultLibraryPath();
    LoadAllPlugins();
    InitWatchDir();
}

void PluginManager::InitWatchDir()
{
    mInotifyFd = mDirWatch.GetInotifyFd();
    mDirWatch.AddDirWatch(mDefaultLibPath.c_str());
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
                    LoadPlugin(pEvent->name);
                }
                if (pEvent->mask & IN_DELETE) {
                    SPR_LOGD("File %s is deleted\n", pEvent->name);
                    UnloadPlugin(pEvent->name);
                }
            }
            offset += sizeof(struct inotify_event) + pEvent->len;
        }
    });

    EpollEventHandler::GetInstance()->AddPoll(mFilePtr.get());
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

    void* pDlHandler = dlopen(path.c_str(), RTLD_NOW);
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
