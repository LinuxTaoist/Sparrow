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

#define POWER_LEV2_MACROS                       \
    ENUM_OR_STRING(LEV2_POWER_ANY),             \
    ENUM_OR_STRING(LEV2_POWER_TO_ACTIVE_ING),   \
    ENUM_OR_STRING(LEV2_POWER_TO_STANDBY_ING),  \
    ENUM_OR_STRING(LEV2_POWER_TO_SLEEP_ING),    \
    ENUM_OR_STRING(LEV2_POWER_BUTT)

// 一级状态:
enum EPowerLev1State
{
    POWER_LEV1_MACROS
};

//二级状态:
enum EPowerLev2State
{
    POWER_LEV2_MACROS
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
    void SetLev2State(EPowerLev2State state);
    EPowerLev2State GetLev2State() { return mCurLev2State; }
    std::string GetLev2String(EPowerLev2State state);

    void HWPowerInit();
    void HWPowerSetMode(int32_t mode);
    void DoBootBusiness();
    void DoResumeBusiness();
    void EnterActive();
    void EnterStandby();
    void EnterSleep();
    void NotifyAllWithStartup();
    void NotifyAllWithStandby();
    void NotifyAllWithSleep();
    void NotifyEvent(uint32_t event);
    void PostAEvent(uint32_t event, void* args = nullptr, int32_t size = 0);
    bool IsAllowStandbyWithAllObserver();
    void ResetAllObserverPreStandbyAck();

    /* 消息响应函数 */
    void MsgRespondObserverRegister(const SprMsg& msg);
    void MsgRespondPowerOn(const SprMsg& msg);
    void MsgRespondStartupPollTimerEvent(const SprMsg& msg);
    void MsgRespondStartupPollTimerEventUnexpected(const SprMsg& msg);
    void MsgRespondPowerOff(const SprMsg& msg);
    void MsgRespondPreStandbyResponse(const SprMsg& msg);
    void MsgRespondPreStandbyResponseTimeout(const SprMsg& msg);
    void MsgRespondPreStandbyResponseUnexpected(const SprMsg& msg);
    void MsgRespondStandbyPollTimerEvent(const SprMsg& msg);
    void MsgRespondStandbyPollTimerEventUnexpected(const SprMsg& msg);
    void MsgRespondEnterSleepTimerEvent(const SprMsg& msg);
    void MsgRespondUnexpectedState(const SprMsg& msg);
    void MsgRespondUnexpectedMsg(const SprMsg& msg);

    /* 注册/注销所有调试函数 */
    void RegisterDebugFuncs();
    void UnregisterDebugFuncs();

    /* 调试函数 */
    void DebugSendPowerOn(const std::vector<std::string>& args);
    void DebugSendPowerOff(const std::vector<std::string>& args);
    void DebugDumpCurState(const std::vector<std::string>& args);
    void DebugDumpObservers(const std::vector<std::string>& args);

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
    EPowerLev1State mCurLev1State;
    EPowerLev2State mCurLev2State;
    InternalDefs::EStartupType mStartupType;
    InternalDefs::EWakeupSourceType mWakeupSourceType;
    InternalDefs::EStandbyReasonType mStandbyReason;
    std::map<uint32_t, StandbyDetail> mStandbyObservers;  // key: module id
};

#endif // __POWER_MANAGER_H__
