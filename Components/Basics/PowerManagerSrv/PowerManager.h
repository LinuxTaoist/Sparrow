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
#include "SprObserverWithMQueue.h"

namespace {

#ifdef ENUM_OR_STRING
#undef ENUM_OR_STRING
#endif
#define ENUM_OR_STRING(x) x

}

#define POWER_LEV1_MACROS               \
    ENUM_OR_STRING(LEV1_POWER_ANY),     \
    ENUM_OR_STRING(LEV1_POWER_INIT),    \
    ENUM_OR_STRING(LEV1_POWER_ACTIVE),  \
    ENUM_OR_STRING(LEV1_POWER_STANDBY), \
    ENUM_OR_STRING(LEV1_POWER_SLEEP),   \
    ENUM_OR_STRING(LEV1_POWER_BUTT)

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

struct StandbyDetail
{
    InternalDefs::EPreStandbyAck preStandbyAck;
    InternalDefs::EModuleBootPriority priority;
};

class PowerManager : public SprObserverWithMQueue
{
public:
    PowerManager(ModuleIDType id, const std::string& name);
    virtual ~PowerManager();

    int32_t Init() override;
    int32_t ProcessMsg(const SprMsg& msg) override;

private:
     /* 更新一级状态 */
    void SetLev1State(EPowerLev1State state);
    EPowerLev1State GetLev1State() { return mCurLev1State; }
    std::string GetLev1String(EPowerLev1State state);

    /* 更新二级状态 */
    void SetLev2State(EPowerLev2State state) { mCurLev2State = state; }
    EPowerLev2State GetLev2State() { return mCurLev2State; }

    void DoBootBusiness();
    void DoResumeBusiness();
    void EnterActive();
    void EnterStandby();
    void EnterSleep();
    void NotifyAllWithStartup();
    void NotifyAllWithStandby();
    void NotifyAllWithSleep();
    void NotifyEvent(uint32_t event);
    bool IsAllowStandbyWithAllObserver();

    /* 消息响应函数 */
    void MsgRespondObserverRegister(const SprMsg& msg);
    void MsgRespondPowerOn(const SprMsg& msg);
    void MsgRespondStartupPollTimerEvent(const SprMsg& msg);
    void MsgRespondPowerOff(const SprMsg& msg);
    void MsgRespondPreStandbyResponse(const SprMsg& msg);
    void MsgRespondPreStandbyResponseTimeout(const SprMsg& msg);
    void MsgRespondStandbyPollTimerEvent(const SprMsg& msg);
    void MsgRespondEnterSleepTimerEvent(const SprMsg& msg);
    void MsgRespondUnexpectedState(const SprMsg& msg);
    void MsgRespondUnexpectedMsg(const SprMsg& msg);

    /* 注册/注销所有调试函数 */
    void RegisterDebugFuncs();
    void UnregisterDebugFuncs();

    /* 调试函数 */
    void DebugDumpCurState(const std::vector<std::string>& args);
    void DebugDumpObservers(const std::vector<std::string>& args);
    void DebugSendPowerOn(const std::vector<std::string>& args);
    void DebugSendPowerOff(const std::vector<std::string>& args);

private:
    using StateTransitionType = InternalDefs::StateTransition<EPowerLev1State,
                                                EPowerLev2State,
                                                InternalDefs::ESprSigId,
                                                PowerManager,
                                                SprMsg>;
    static std::vector<StateTransitionType> mStateTable;

    bool mPreStandbyResponseTimer;
    int32_t mStandbyTimerCnt;
    uint32_t mCurNotifyStartupEvent;
    uint32_t mCurNotifyStandbyEvent;
    InternalDefs::EStartupType mStartupType;
    EPowerLev1State mCurLev1State;
    EPowerLev2State mCurLev2State;
    std::map<uint32_t, StandbyDetail> mStandbyObservers;  // key: module id
};

#endif // __POWER_MANAGER_H__
