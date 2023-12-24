/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PowerManager.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/12/21
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *------------------------------------------------------------------------------
 *  2023/12/21 | 1.0.0.1   | Xiang.D        | Create file
 *------------------------------------------------------------------------------
 *
 */

#ifndef __POWER_MANAGER_H__
#define __POWER_MANAGER_H__

#include "SprObserver.h"

namespace InternalEnum {

enum EPowerState
{
    POWER_STATE_NORMAL,
    POWER_STATE_STANDBY,
    POWER_STATE_SLEEP,
    POWER_STATE_WAKEUP,
    POWER_STATE_BUTT
};


};

template <class Lev1State, class Lev2State, class SignalType, class Instance, class MsgType>
struct StateTransition
{
    Lev1State   lev1State;
    Lev2State   lev2State;
    SignalType	msgId;
    void (Instance::*callback)(SprMsg *msg);
};

class PowerManager : public SprObserver
{
public:
    PowerManager(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr);
    virtual ~PowerManager();

    int ProcessMsg(const SprMsg& msg);
};

#endif
