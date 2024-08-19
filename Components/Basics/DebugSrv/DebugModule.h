/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DebugModule.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/28
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/28 | 1.0.0.1  | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __DEBUG_MODULE_H__
#define __DEBUG_MODULE_H__

#include "SprLog.h"
#include "RemoteShell.h"
#include "SprObserverWithMQueue.h"

class DebugModule : public SprObserverWithMQueue
{
public:
    DebugModule(ModuleIDType id, const std::string& name);
    ~DebugModule();

    int  ProcessMsg(const SprMsg& msg) override;

private:
    int MsgRespondEnableRemoteShell(const SprMsg& msg);
    int MsgRespondDisableRemoteShell(const SprMsg& msg);
    int MsgRespondDispatchTimerMsg(const SprMsg& msg);
    int MsgRespondBroadcastMsg(const SprMsg& msg);

private:
    RemoteShell mShell;
};

#endif // __DEBUG_MODULE_H__
