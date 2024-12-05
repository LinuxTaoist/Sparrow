/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PluginManager.h
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
#ifndef __PLUGIN_MANAGER_H__
#define __PLUGIN_MANAGER_H__

#include <map>
#include <vector>
#include <string>
#include <memory>
#include "PFile.h"
#include "SprContext.h"
#include "SprDirWatch.h"

class PluginManager
{
public:
    PluginManager();
    ~PluginManager();

    void Init();
    void SetHotPlugEnable(bool enable) { mHotPlugEnable = enable; }

private:
    void InitWatchDir();
    void LoadPlugin(const std::string& path);
    void UnloadPlugin(const std::string& path);
    void LoadAllPlugins();
    void UnloadAllPlugins();
    std::string GetDefaultLibraryPath();

private:
    bool mHotPlugEnable;
    SprContext mContext;
    SprDirWatch mDirWatch;
    std::string mDefaultLibPath;
    std::shared_ptr<PFile> mFilePtr;
    std::map<std::string, void*> mPluginHandles;
    std::map<int, SprObserver*> mPluginModules;
};

#endif // __PLUGIN_MANAGER_H__
