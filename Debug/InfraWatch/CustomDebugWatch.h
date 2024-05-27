/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CustomDebugWatch.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/04/23
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/04/23 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __CUSTOM_DEBUG_WATCH_H__
#define __CUSTOM_DEBUG_WATCH_H__

#include <thread>
#include "SprObserver.h"
#include "InfraCommon.h"

class DebugModule : public SprObserver
{
public:
    DebugModule(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr);
    ~DebugModule();

    int ProcessMsg(const SprMsg& msg) override;
    int AddTimerInOneSec();
    int DelTimerInOneSec();
    int AddCustomTimer();
    int DelCustomTimer();
};

class CustomDebugWatch
{
public:
    CustomDebugWatch();
    ~CustomDebugWatch();

    char MenuEntry();

private:
    char HandleInputInMenu(char input);

private:
    std::thread mEpollThread;
    std::shared_ptr<DebugModule> mDebugModulePtr;
};

extern CustomDebugWatch theCustomDebugWatch;

#endif // __CUSTOM_DEBUG_WATCH_H__
