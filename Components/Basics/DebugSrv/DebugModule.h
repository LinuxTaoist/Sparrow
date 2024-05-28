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
#include "SprObserver.h"

class DebugModule : public SprObserver
{
public:
    DebugModule(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr);
    ~DebugModule();

    int ProcessMsg(const SprMsg& msg) override;

private:
    int MsgRespondDispatchTimerMsg(const SprMsg& msg);
    int MsgRespondBroadcastMsg(const SprMsg& msg);
};

#endif // __DEBUG_MODULE_H__
