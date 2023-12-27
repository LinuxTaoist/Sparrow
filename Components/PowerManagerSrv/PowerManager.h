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

#include <vector>
#include "SprObserver.h"

namespace SprPower {

// 一级状态:
enum EPowerLev1State
{
    LEV1_POWER_ANY      = 0x00,
    LEV1_POWER_WORKING,
    LEV1_POWER_STANDBY,
    LEV1_POWER_SLEEP
};

//二级状态:
enum EPowerLev2State
{
    LEV2_POWER_ANY      = 0x00
};

template <class Lev1State, class Lev2State, class SignalType, class ClassName, class MsgType>
struct StateTransition
{
    Lev1State   lev1State;
    Lev2State   lev2State;
    SignalType	sigId;
    void (ClassName::*callback)(const MsgType& msg);
};

class PowerManager : public SprObserver
{
public:
    PowerManager(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr);
    virtual ~PowerManager();

    int ProcessMsg(const SprMsg& msg);

private:
    static std::vector<StateTransition <EPowerLev1State,
                        EPowerLev2State,
                        InternalEnum::ESprSigId,
                        PowerManager,
                        SprMsg> > mStateTable;

    EPowerLev1State mCurLev1State;
    EPowerLev2State mCurLev2State;

     /* 更新一级状态 */
    void SetLev1State(EPowerLev1State state) { mCurLev1State = state; }
    EPowerLev1State GetLev1State() { return mCurLev1State; }

    /* 更新二级状态 */
    void SetLev2State(EPowerLev2State state) { mCurLev2State = state; }
    EPowerLev2State GetLev2State() { return mCurLev2State; }
};
} // namespace SprPower
#endif
