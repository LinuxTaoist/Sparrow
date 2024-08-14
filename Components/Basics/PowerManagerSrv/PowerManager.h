/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PowerManager.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/12/21
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/12/21 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __POWER_MANAGER_H__
#define __POWER_MANAGER_H__

#include <vector>
#include "SprObserver.h"

#define POWER_LEV1_MACROS \
    ENUM_OR_STRING(LEV1_POWER_ANY),     \
    ENUM_OR_STRING(LEV1_POWER_INIT),    \
    ENUM_OR_STRING(LEV1_POWER_ACTIVE),  \
    ENUM_OR_STRING(LEV1_POWER_STANDBY), \
    ENUM_OR_STRING(LEV1_POWER_SLEEP),   \
    ENUM_OR_STRING(LEV1_POWER_BUTT)

#ifdef ENUM_OR_STRING
#undef ENUM_OR_STRING
#endif
#define ENUM_OR_STRING(x) x

// 一级状态:
enum EPowerLev1State
{
    POWER_LEV1_MACROS
};

//二级状态:
enum EPowerLev2State
{
    LEV2_POWER_ANY      = 0x00
};

class PowerManager : public SprObserver
{
public:
    PowerManager(ModuleIDType id, const std::string& name);
    virtual ~PowerManager();

    int ProcessMsg(const SprMsg& msg) override;

private:
     /* 更新一级状态 */
    void SetLev1State(EPowerLev1State state);
    EPowerLev1State GetLev1State() { return mCurLev1State; }
    std::string GetLev1String(EPowerLev1State lev1);

    /* 更新二级状态 */
    void SetLev2State(EPowerLev2State state) { mCurLev2State = state; }
    EPowerLev2State GetLev2State() { return mCurLev2State; }

    void PerformBootBusiness();
    void PerformResumeBusiness();
    void PerformStandbyBusiness();
    void PerformSleepBusiness();
    void BroadcastPowerEvent(uint32_t event);

    /* 消息响应函数 */
    void MsgRespondPowerOnWithInit(const SprMsg& msg);
    void MsgRespondPowerOnWithStandby(const SprMsg& msg);
    void MsgRespondPowerOnWithSleep(const SprMsg& msg);
    void MsgRespondPowerOffWithActive(const SprMsg& msg);
    void MsgRespondUnexpectedMsg(const SprMsg& msg);

private:
    using StateTransitionType = InternalDefs::StateTransition<EPowerLev1State,
                                                EPowerLev2State,
                                                InternalDefs::ESprSigId,
                                                PowerManager,
                                                SprMsg>;
    static std::vector<StateTransitionType> mStateTable;

    EPowerLev1State mCurLev1State;
    EPowerLev2State mCurLev2State;
};

#endif // __POWER_MANAGER_H__
