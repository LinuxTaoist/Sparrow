/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprSystem.h
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
#ifndef __SPR_SYSTEM_H__
#define __SPR_SYSTEM_H__

#include <map>
#include <vector>
#include <string>
#include "SprObserver.h"
#include "CommonTypeDefs.h"

class SprSystem
{
public:
    // --------------------------------------------------------------------------------------------
    // - Init functions
    // --------------------------------------------------------------------------------------------
    ~SprSystem();
    SprSystem(const SprSystem&) = delete;
    SprSystem& operator=(const SprSystem&) = delete;
    void Init();

    // --------------------------------------------------------------------------------------------
    // - Single instance
    // --------------------------------------------------------------------------------------------
    static SprSystem* GetInstance();

private:
    SprSystem();
    void InitEnv();
    void InitMsgQueueLimit();
    void LoadReleaseInformation();
    void LoadPlugins();
    void ReleasePlugins();
    int EnvReady(const std::string& srvName);

private:
    std::vector<void*> mPluginHandles;
    std::vector<PluginEntryFunc> mPluginEntries;
    std::map<int, SprObserver*> mModules;
};

#endif // __SPR_SYSTEM_H__
