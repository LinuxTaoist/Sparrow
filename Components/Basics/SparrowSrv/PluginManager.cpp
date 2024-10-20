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
#include "SprLog.h"
#include "CoreTypeDefs.h"
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

}

void PluginManager::Init()
{

}

void PluginManager::PluginHotInsert(const std::string& path)
{

}

void PluginManager::PluginHotRemove(const std::string& path)
{

}

void PluginManager::LoadPlugin(const std::string& path)
{
    if (strncmp(path.c_str(), DEFAULT_PLUGIN_LIBRARY_FILE_PREFIX, strlen(DEFAULT_PLUGIN_LIBRARY_FILE_PREFIX)) != 0) {
        return;
    }

    void* pDlHandler = dlopen(path.c_str(), RTLD_NOW);
    if (!pDlHandler) {
        SPR_LOGE("Load plugin %s fail! (%s)\n", path.c_str(), dlerror() ? dlerror() : "unknown error");
        return;
    }

    auto pEntry = (PluginEntryFunc)dlsym(pDlHandler, DEFAULT_PLUGIN_LIBRARY_ENTRY_FUNC);
    if (!pEntry) {
        SPR_LOGE("Find %s fail in %s! (%s)\n", DEFAULT_PLUGIN_LIBRARY_ENTRY_FUNC, path.c_str(), dlerror() ? dlerror() : "unknown error");
        dlclose(pDlHandler);
        return;
    }

    mPluginHandles[path] = pDlHandler;
    SPR_LOGD("Load plugin %s success!\n", path.c_str());
}

void PluginManager::LoadAllPlugins()
{

}
