/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PowerManager.cpp
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
#include <vector>
#include <algorithm>
#include "SprLog.h"
#include "SprDebugNode.h"
#include "PowerManager.h"
#include "SprEnumHelper.h"

using namespace std;
using namespace InternalDefs;

#define LOG_TAG "PowerM"

#define STANDBY_RESPONSE_TIMEOUT        2000
#define STANDBY_RESPONSE_IIMEOUT_TOTAL  4000
#define STANDBY_POLL_EVENT_400MS        400

vector <StateTransition <   EPowerLev1State,
                            EPowerLev2State,
                            ESprSigId,
                            PowerManager,
                            SprMsg> >
PowerManager::mStateTable =
{
    // =============================================================
    // All States for SIG_ID_POWER_ON
    // =============================================================
    { LEV1_POWER_INIT, LEV2_POWER_ANY,
      SIG_ID_POWER_ON,
      &PowerManager::MsgRespondPowerOn
    },

    { LEV1_POWER_STANDBY, LEV2_POWER_ANY,
      SIG_ID_POWER_ON,
      &PowerManager::MsgRespondPowerOn
    },

    { LEV1_POWER_SLEEP, LEV2_POWER_ANY,
      SIG_ID_POWER_ON,
      &PowerManager::MsgRespondPowerOn
    },

    { LEV1_POWER_ANY, LEV2_POWER_ANY,
      SIG_ID_POWER_ON,
      &PowerManager::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_POWER_STARTUP_POLL_TIMER_EVENT
    // =============================================================
    { LEV1_POWER_ACTIVE, LEV2_POWER_ANY,
      SIG_ID_POWER_STARTUP_POLL_TIMER_EVENT,
      &PowerManager::MsgRespondStartupPollTimerEvent
    },

    { LEV1_POWER_ANY, LEV2_POWER_ANY,
      SIG_ID_POWER_STARTUP_POLL_TIMER_EVENT,
      &PowerManager::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_POWER_OFF
    // =============================================================
    { LEV1_POWER_ACTIVE, LEV2_POWER_ANY,
      SIG_ID_POWER_OFF,
      &PowerManager::MsgRespondPowerOff
    },

    { LEV1_POWER_ANY, LEV2_POWER_ANY,
      SIG_ID_POWER_OFF,
      &PowerManager::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_POWER_STANDBY_RESPONSE
    // =============================================================
    { LEV1_POWER_ACTIVE, LEV2_POWER_ANY,
      SIG_ID_POWER_PRE_STANDBY_RESPONSE,
      &PowerManager::MsgRespondPreStandbyResponse
    },

    { LEV1_POWER_ANY, LEV2_POWER_ANY,
      SIG_ID_POWER_PRE_STANDBY_RESPONSE,
      &PowerManager::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_POWER_PRE_STANDBY_RESPONSE_TIMEOUT
    // =============================================================
    { LEV1_POWER_ACTIVE, LEV2_POWER_ANY,
      SIG_ID_POWER_PRE_STANDBY_RESPONSE_TIMEOUT,
      &PowerManager::MsgRespondPreStandbyResponseTimeout
    },

    { LEV1_POWER_ANY, LEV2_POWER_ANY,
      SIG_ID_POWER_PRE_STANDBY_RESPONSE_TIMEOUT,
      &PowerManager::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_POWER_STANDBY_POLL_TIMER_EVENT
    // =============================================================
    { LEV1_POWER_ACTIVE, LEV2_POWER_ANY,
      SIG_ID_POWER_STANDBY_POLL_TIMER_EVENT,
      &PowerManager::MsgRespondStandbyPollTimerEvent
    },

    { LEV1_POWER_ANY, LEV2_POWER_ANY,
      SIG_ID_POWER_STANDBY_POLL_TIMER_EVENT,
      &PowerManager::MsgRespondUnexpectedState
    },

    // =============================================================
    // Default case for handling unexpected messages,
    // mandatory to denote end of message table.
    // =============================================================
    { LEV1_POWER_ANY, LEV2_POWER_ANY,
      SIG_ID_ANY,
      &PowerManager::MsgRespondUnexpectedMsg
    }
};

PowerManager::PowerManager(ModuleIDType id, const std::string& name)
            : SprObserverWithMQueue(id, name)
{
    mEnableStandbyTimer = false;
    mStandbyTimerCnt = 0;
    mCurNotifyStartupEvent = SIG_ID_ANY;
    mCurNotifyStandbyEvent = SIG_ID_ANY;
    mStartupType = STARTUP_BUTT;
    SetLev1State(LEV1_POWER_INIT);
    SetLev2State(LEV2_POWER_ANY);
}

PowerManager::~PowerManager()
{
    UnregisterDebugFuncs();
}

int32_t PowerManager::Init()
{
    RegisterDebugFuncs();
    return 0;
}

std::string PowerManager::GetLev1String(EPowerLev1State state)
{
    #ifdef ENUM_OR_STRING
    #undef ENUM_OR_STRING
    #endif
    #define ENUM_OR_STRING(x) #x

    static std::vector<std::string> Lev1Strings = {
        POWER_LEV1_MACROS
    };

    return (Lev1Strings.size() > state) ? Lev1Strings[state] : "UNDEFINED";
}

void PowerManager::SetLev1State(EPowerLev1State state)
{
    SPR_LOGD("State changed: %s -> %s\n",
        GetLev1String(mCurLev1State).c_str(), GetLev1String(state).c_str());

    mCurLev1State = state;
}

void PowerManager::DoBootBusiness()
{
    SPR_LOGD("Do boot business!\n");
}

void PowerManager::DoResumeBusiness()
{
    SPR_LOGD("Do resume business!\n");
}

void PowerManager::NotifyAllWithStartup()
{
    // Notify startup event with poriority
    mCurNotifyStartupEvent = SIG_ID_POWER_STARTUP_HIGHEST;
    RegisterTimer(0, STANDBY_POLL_EVENT_400MS, SIG_ID_POWER_STARTUP_POLL_TIMER_EVENT, 0);
}

void PowerManager::NotifyAllWithStandby()
{
    // Notify stanby event with poriority
    mCurNotifyStandbyEvent = SIG_ID_POWER_STANDBY_HIGHEST;
    RegisterTimer(0, STANDBY_POLL_EVENT_400MS, SIG_ID_POWER_STANDBY_POLL_TIMER_EVENT, 0);
}

void PowerManager::NotifyAllWithSleep()
{
    SetLev1State(LEV1_POWER_SLEEP);
    NotifyEvent(SIG_ID_POWER_SLEEP);
}

void PowerManager::NotifyEvent(uint32_t event)
{
    SprMsg msg(event);
    NotifyAllObserver(msg);
    SPR_LOGD("Broadcast power event: %s\n", GetSigName(event));
}

/**
 * @brief Process SIG_ID_POWER_ON
 *
 * @param[in] msg
 * @return none
 */
void PowerManager::MsgRespondPowerOn(const SprMsg& msg)
{
    mStartupType = (EStartupType)msg.GetI32Value();
    SPR_LOGD("Receice power on, startup type: 0x%x (%s)!\n", GetStartupTypeText(mStartupType).c_str());

    if (mStartupType == STARTUP_COLD_BOOT) {
        DoBootBusiness();
    } else {
        DoResumeBusiness();
    }

    SetLev1State(LEV1_POWER_ACTIVE);
    NotifyAllWithStartup();
}

/**
 * @brief Process SIG_ID_POWER_STARTUP_POLL_TIMER_EVENT
 *
 * @param[in] msg
 * @return none
 */
void PowerManager::MsgRespondStartupPollTimerEvent(const SprMsg& msg)
{
    if (mCurNotifyStartupEvent > SIG_ID_POWER_STARTUP_LOWEST) {
        SPR_LOGD("Send all startup events finished!\n");
        UnregisterTimer(SIG_ID_POWER_STARTUP_POLL_TIMER_EVENT);
        return;
    }

    NotifyEvent(mCurNotifyStartupEvent);
    mCurNotifyStartupEvent++;
}

/**
 * @brief Process SIG_ID_POWER_OFF
 *
 * @param[in] msg
 * @return none
 */
void PowerManager::MsgRespondPowerOff(const SprMsg& msg)
{
    // 1. Send SIG_ID_POWER_PRE_STANDBY_REQUEST to all modules
    // 2. Wait SIG_ID_POWER_STANDBY_RESPONSE from all modules with stanby allowed in 2s timeout
    // 3. Receive SIG_ID_POWER_STANDBY_RESPONSE
    //    If refuse from some modules, unregister timer, not standby and over
    //    If delay from some modules, resend SIG_ID_POWER_PRE_STANDBY_REQUEST on 2s timeout
    //    If all modules are allowed to standby, unregister timer, send SIG_ID_POWER_STANDBY to
    //       all modules with priority
    // 6. If not received refuse and 6s timeout, unregister timer, send SIG_ID_POWER_STANDBY to
    //       all modules with priority
    // 7. After enter standby in n sec timeout, send SIG_ID_POWER_SLEEP to all modules
    SPR_LOGD("Handle power off with %s!\n", GetLev1String(mCurLev1State).c_str());
    NotifyEvent(SIG_ID_POWER_PRE_STANDBY_REQUEST);

    mEnableStandbyTimer = true;
    mStandbyTimerCnt = 0;
    RegisterTimer(0, STANDBY_RESPONSE_TIMEOUT, SIG_ID_POWER_PRE_STANDBY_RESPONSE_TIMEOUT, 0);
}

/**
 * @brief Process SIG_ID_POWER_PRE_STANDBY_RESPONSE
 *
 * @param[in] msg
 * @return none
 */
void PowerManager::MsgRespondPreStandbyResponse(const SprMsg& msg)
{
    uint32_t moduleID = msg.GetFrom();
    int32_t ack = msg.GetI32Value();
    SPR_LOGD("Receive %s from %s!\n",
        GetSprPreStandbyAckText(ack).c_str(),
        GetSprModuleIDText(moduleID).c_str());

    if (ack == PRE_STANDBY_ACK_DELAY) {
        SPR_LOGD("Delay standby!\n");
        return;
    }

    for (auto &comp : mStandbyComponents) {
        if (comp.first == moduleID) {
            comp.second.preStandbyAck = (EPreStandbyAck)ack;
        }

        if (comp.second.preStandbyAck != PRE_STANDBY_ACK_ALLOW) {
            SPR_LOGD("%s not allow standby (%s)!\n",
                GetSprModuleIDText(moduleID).c_str(),
                GetSprPreStandbyAckText(comp.second.preStandbyAck).c_str());
            return;
        }
    }

    if (ack == PRE_STANDBY_ACK_ALLOW) {
        SPR_LOGD("All modules are allowed to standby!\n");
        NotifyAllWithStandby();
    }

    // Unregister timer, When all modules are allowed to standby or
    // some modules refuse standby
    mEnableStandbyTimer = false;
    UnregisterTimer(SIG_ID_POWER_PRE_STANDBY_RESPONSE_TIMEOUT);
}

/**
 * @brief Process SIG_ID_POWER_PRE_STANDBY_RESPONSE_TIMEOUT
 *
 * @param[in] msg
 * @return none
 */
void PowerManager::MsgRespondPreStandbyResponseTimeout(const SprMsg& msg)
{
    // 1. Request all components to see if they are allowed to standby
    mStandbyTimerCnt++;
    if ((mStandbyTimerCnt * STANDBY_RESPONSE_TIMEOUT) <= STANDBY_RESPONSE_IIMEOUT_TOTAL) {
        SPR_LOGD("resend SIG_ID_POWER_PRE_STANDBY_REQUEST, timeout = %dms cnt = %d\n",
            STANDBY_RESPONSE_TIMEOUT, mStandbyTimerCnt);

        NotifyEvent(SIG_ID_POWER_PRE_STANDBY_REQUEST);
        return;
    }

    // 2. If the response timeout reaches 4s, start standby
    SPR_LOGD("Total timeout over %dms, do standby business\n", mStandbyTimerCnt * STANDBY_RESPONSE_TIMEOUT);
    NotifyAllWithStandby();
    mEnableStandbyTimer = false;
    UnregisterTimer(SIG_ID_POWER_PRE_STANDBY_RESPONSE_TIMEOUT);
}

/**
 * @brief Process SIG_ID_POWER_STANDBY_POLL_TIMER_EVENT
 *
 * @param[in] msg
 * @return none
 */
void PowerManager::MsgRespondStandbyPollTimerEvent(const SprMsg& msg)
{
    if (mCurNotifyStandbyEvent > SIG_ID_POWER_STANDBY_LOWEST) {
        SPR_LOGD("Send all standby events finished!\n");
        SetLev1State(LEV1_POWER_STANDBY);
        UnregisterTimer(SIG_ID_POWER_STANDBY_POLL_TIMER_EVENT);
        return;
    }

    NotifyEvent(mCurNotifyStandbyEvent);
    mCurNotifyStandbyEvent++;
}

/**
 * @brief Process
 *
 * @param[in] msg
 * @return none
 */
void PowerManager::MsgRespondUnexpectedState(const SprMsg& msg)
{
    SPR_LOGW("Unexpected state: msg = %s on %s\n",
        GetSigName(msg.GetMsgId()), GetLev1String(mCurLev1State).c_str());
}

/**
 * @brief Process
 *
 * @param[in] msg
 * @return none
 */
void PowerManager::MsgRespondUnexpectedMsg(const SprMsg& msg)
{
    SPR_LOGW("Unexpected msg: msg = %s on %s\n",
                GetSigName(msg.GetMsgId()), GetLev1String(mCurLev1State).c_str());
}

int PowerManager::ProcessMsg(const SprMsg& msg)
{
    // SPR_LOGD("Recv msg: %s on %s\n", GetSigName(msg.GetMsgId()), GetLev1String(mCurLev1State).c_str());
    auto stateEntry = std::find_if(mStateTable.begin(), mStateTable.end(),
        [this, &msg](const StateTransitionType& entry) {
            return ((entry.lev1State  == mCurLev1State  || entry.lev1State  == LEV1_POWER_ANY) &&
                    (entry.lev2State  == mCurLev2State  || entry.lev2State  == LEV2_POWER_ANY) &&
                    (entry.sigId      == msg.GetMsgId() || entry.sigId      == SIG_ID_ANY) );
        });

    if (stateEntry != mStateTable.end()) {
        (this->*(stateEntry->callback))(msg);
    }

    return 0;
}

void PowerManager::RegisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    p->RegisterCmd(mModuleName, "DumpCurState",     "Dump current state",   std::bind(&PowerManager::DebugDumpCurState,  this, std::placeholders::_1));
    p->RegisterCmd(mModuleName, "PowerOn",          "Send power on",        std::bind(&PowerManager::DebugSendPowerOn,   this, std::placeholders::_1));
    p->RegisterCmd(mModuleName, "PowerOff",         "Send power off",       std::bind(&PowerManager::DebugSendPowerOff,  this, std::placeholders::_1));
}
void PowerManager::UnregisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    SPR_LOGD("Unregister %s all debug funcs\n", mModuleName.c_str());
    p->UnregisterCmd(mModuleName);
}

void PowerManager::DebugDumpCurState(const std::vector<std::string>& args)
{
    SPR_LOGD("Lev1State: %s\n", GetLev1String(mCurLev1State).c_str());
}

void PowerManager::DebugSendPowerOn(const std::vector<std::string>& args)
{
    SendMsg(SIG_ID_POWER_ON);
}

void PowerManager::DebugSendPowerOff(const std::vector<std::string>& args)
{
    SendMsg(SIG_ID_POWER_OFF);
}
