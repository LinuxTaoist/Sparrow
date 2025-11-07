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
#include "AsyncEvent.h"
#include "SprDebugNode.h"
#include "PowerManager.h"
#include "SprEnumHelper.h"
#include "CommonErrorCodes.h"

using namespace std;
using namespace InternalDefs;

#define LOG_TAG "PowerM"

#define STANDBY_RESPONSE_TIMEOUT        2000
#define STANDBY_RESPONSE_IIMEOUT_TOTAL  6000
#define STANDBY_POLL_EVENT_400MS        400
#define STANDBY_ENTER_SLEEP_TIMEOUT     5000

vector <StateTransition <   EPowerLev1State,
                            EPowerLev2State,
                            ESprSigId,
                            PowerManager,
                            SprMsg> >
PowerManager::mStateTable =
{
    // =============================================================
    // All States for SIG_ID_POWER_OBSERVER_REGISTER
    // =============================================================
    { LEV1_POWER_ANY, LEV2_POWER_ANY,
      SIG_ID_POWER_OBSERVER_REGISTER,
      &PowerManager::MsgRespondObserverRegister
    },

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
    // All States for SIG_ID_POWER_ENTER_SLEEP_TIMER_EVENT
    { LEV1_POWER_STANDBY, LEV2_POWER_ANY,
      SIG_ID_POWER_ENTER_SLEEP_TIMER_EVENT,
      &PowerManager::MsgRespondEnterSleepTimerEvent
    },

    { LEV1_POWER_ANY, LEV2_POWER_ANY,
      SIG_ID_POWER_ENTER_SLEEP_TIMER_EVENT,
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
    mPreStandbyResponseTimer = false;
    mStandbyTimerCnt = 0;
    mCurNotifyStartupEvent = SIG_ID_ANY;
    mCurNotifyStandbyEvent = SIG_ID_ANY;
    mStartupType = STARTUP_BUTT;
    mWakeupSourceType = WAKEUP_SOURCE_BUTT;
    mStandbyReason = STANDBY_REASON_BUTT;
    mCurLev1State = LEV1_POWER_INIT;
    mCurLev2State = LEV2_POWER_ANY;
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

void PowerManager::EnterActive()
{
    SPR_LOGD("Enter active!\n");
    SetLev1State(LEV1_POWER_ACTIVE);
    NotifyAllWithStartup();

    // Post active event to external
    PostAEvent(POWER_MGR_ACTIVE);
}

void PowerManager::EnterStandby()
{
    SPR_LOGD("Enter standby!\n");
    SetLev1State(LEV1_POWER_STANDBY);
    UnregisterTimer(SIG_ID_POWER_STANDBY_POLL_TIMER_EVENT);

    // Post standby event to external
    PostAEvent(POWER_MGR_STANDBY);

    // Enter sleep after 5s in standby state
    RegisterTimer(STANDBY_ENTER_SLEEP_TIMEOUT, STANDBY_ENTER_SLEEP_TIMEOUT, SIG_ID_POWER_ENTER_SLEEP_TIMER_EVENT, 1);
}

void PowerManager::EnterSleep()
{
    SPR_LOGD("Enter sleep!\n");
    SetLev1State(LEV1_POWER_SLEEP);
    NotifyAllWithSleep();

    // Post sleep event to external
    PostAEvent(POWER_MGR_SLEEP);
}

void PowerManager::NotifyAllWithStartup()
{
    // Notify startup event with priority
    mCurNotifyStartupEvent = SIG_ID_POWER_STARTUP_HIGHEST - 1;
    RegisterTimer(0, STANDBY_POLL_EVENT_400MS, SIG_ID_POWER_STARTUP_POLL_TIMER_EVENT, 0);
}

void PowerManager::NotifyAllWithStandby()
{
    // Notify stanby event with priority
    mCurNotifyStandbyEvent = SIG_ID_POWER_STANDBY_HIGHEST - 1;
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

void PowerManager::PostAEvent(uint32_t event, void* args, int32_t size)
{
    int32_t ret = POST_AEVENT(event, args, size);
    SPR_LOGI("Post event (%u) %s\n", event, (ret == 0) ? "success" : "failed");
}

bool PowerManager::IsAllowStandbyWithAllObserver()
{
    bool ret = true;
    for (const auto &obs : mStandbyObservers) {
        if (obs.second.preStandbyAck != PRE_STANDBY_ACK_ALLOW) {
            SPR_LOGD("%s not allow standby (%s)!\n", GetSprModuleIDText(static_cast<ModuleIDType>(obs.first)).c_str(),
                GetSprPreStandbyAckText(obs.second.preStandbyAck).c_str());
            ret = false;
            break;
        }
    }

    return ret;
}

/**
 * @brief Process SIG_ID_POWER_OBSERVER_REGISTER
 *
 * @param[in] msg
 * @return none
 */
void PowerManager::MsgRespondObserverRegister(const SprMsg& msg)
{
    StandbyDetail detail;
    detail.preStandbyAck = PRE_STANDBY_ACK_BUTT;
    detail.priority = (EModuleBootPriority)msg.GetI32Value();
    mStandbyObservers[msg.GetFrom()] = detail;

    SPR_LOGD("Register observer: module = %s, priority = %d (%s)\n",
        GetSprModuleIDText(msg.GetFrom()).c_str(),
        msg.GetI32Value(), GetSprModuleBootPriorityText(detail.priority).c_str());
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
    mWakeupSourceType = (EWakeupSourceType)msg.GetU32Value();
    SPR_LOGD("Receive power on, type = 0x%x (%s), source = 0x%x (%s)!\n",
            mStartupType, GetStartupTypeText(mStartupType).c_str(),
            mWakeupSourceType, GetWakeupSourceTypeText(mWakeupSourceType).c_str());

    if (mStartupType == STARTUP_COLD_BOOT) {
        DoBootBusiness();
    } else {
        DoResumeBusiness();
    }

    EnterActive();
}

/**
 * @brief Process SIG_ID_POWER_STARTUP_POLL_TIMER_EVENT
 *
 * @param[in] msg
 * @return none
 */
void PowerManager::MsgRespondStartupPollTimerEvent(const SprMsg& msg)
{
    // Why mCurNotifyStartupEvent++ first, then send event:
    // When dump mCurNotifyStartupEvent, the value is the same as the event to be sent.
    mCurNotifyStartupEvent++;

    // Finish all priority startup events
    if (mCurNotifyStartupEvent > SIG_ID_POWER_STARTUP_LOWEST) {
        SPR_LOGD("Send all startup events finished!\n");
        UnregisterTimer(SIG_ID_POWER_STARTUP_POLL_TIMER_EVENT);
        return;
    }

    SPR_LOGD("Send startup event: %s\n", GetSigName(mCurNotifyStartupEvent));
    SprMsg msgEvent(mCurNotifyStartupEvent);
    msgEvent.SetI32Value(mStartupType);
    NotifyAllObserver(msgEvent);
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
    mStandbyReason = (EStandbyReasonType)msg.GetI32Value();
    SPR_LOGD("Receive power off, reason = 0x%x (%s)!\n",
              mStandbyReason, GetStandbyReasonTypeText(mStandbyReason).c_str());

    NotifyEvent(SIG_ID_POWER_PRE_STANDBY_REQUEST);
    mPreStandbyResponseTimer = true;
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
    const uint32_t moduleID = msg.GetFrom();
    const int32_t ack = msg.GetI32Value();
    const string ackText = GetSprPreStandbyAckText(ack);
    const string moduleIDText = GetSprModuleIDText(moduleID);

    SPR_LOGD("Receive %s from %s!\n", ackText.c_str(), moduleIDText.c_str());

    auto observer = std::find_if(mStandbyObservers.begin(), mStandbyObservers.end(),
        [moduleID](const std::pair<const uint32_t, StandbyDetail>& obs) {
            return obs.first == moduleID;
        }
    );

    if (observer == mStandbyObservers.end()) {
        SPR_LOGW("Ignore observer: %s\n", moduleIDText.c_str());
        return;
    }

    observer->second.preStandbyAck = static_cast<EPreStandbyAck>(ack);

    bool unregisterTimer = false;
    switch (ack) {
        case PRE_STANDBY_ACK_ALLOW:
            SPR_LOGD("Allow standby from %s!\n", moduleIDText.c_str());
            if (IsAllowStandbyWithAllObserver()) {
                SPR_LOGD("All observers allow to standby!\n");
                unregisterTimer = true;
                NotifyAllWithStandby();
            }
            break;
        case PRE_STANDBY_ACK_REFUSE:
            SPR_LOGD("Refuse standby from %s!\n", moduleIDText.c_str());
            unregisterTimer = true;
            SendEventToMonitor(ERR_POWERM_REFUSE_STANDBY, "Refuse standby (from " + moduleIDText + ")");
            break;
        case PRE_STANDBY_ACK_DELAY:
            SPR_LOGD("Delay standby from %s!\n", moduleIDText.c_str());
            SendEventToMonitor(ERR_POWERM_DELAY_STANDBY, "Delay standby (from " + moduleIDText + ")");
            break;
        default:
            SPR_LOGE("Invalid ack %d from %s!\n", ack, moduleIDText.c_str());
            break;
    }

    // Unregister timer, when all modules are allowed to standby or
    // some modules refuse standby
    if (unregisterTimer) {
        mPreStandbyResponseTimer = false;
        UnregisterTimer(SIG_ID_POWER_PRE_STANDBY_RESPONSE_TIMEOUT);
    }
}

/**
 * @brief Process SIG_ID_POWER_PRE_STANDBY_RESPONSE_TIMEOUT
 *
 * @param[in] msg
 * @return none
 */
void PowerManager::MsgRespondPreStandbyResponseTimeout(const SprMsg& msg)
{
    // 1. Request all obsonents to see if they are allowed to standby
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
    mPreStandbyResponseTimer = false;
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
    // Why mCurNotifyStandbyEvent++ first, then send event:
    // When dump mCurNotifyStandbyEvent, the value is the same as the event to be sent.
    mCurNotifyStandbyEvent++;

    // Finish all priority standby events
    if (mCurNotifyStandbyEvent > SIG_ID_POWER_STANDBY_LOWEST) {
        SPR_LOGD("Enter standby after send all standby events!\n");
        EnterStandby();
        return;
    }

    SPR_LOGD("Send standby event: %s\n", GetSigName(mCurNotifyStandbyEvent));
    SprMsg msgEvent(mCurNotifyStandbyEvent);
    NotifyAllObserver(msgEvent);
}

/**
 * @brief Process SIG_ID_POWER_ENTER_SLEEP_TIMER_EVENT
 *
 * @param msg
 */
void PowerManager::MsgRespondEnterSleepTimerEvent(const SprMsg& msg)
{
    EnterSleep();
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

int32_t PowerManager::ProcessMsg(const SprMsg& msg)
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

    p->RegisterCmd(mModuleName, "PowerOn",          "Send power on",        std::bind(&PowerManager::DebugSendPowerOn,   this, std::placeholders::_1));
    p->RegisterCmd(mModuleName, "PowerOff",         "Send power off",       std::bind(&PowerManager::DebugSendPowerOff,  this, std::placeholders::_1));
    p->RegisterCmd(mModuleName, "DumpCurState",     "Dump current state",   std::bind(&PowerManager::DebugDumpCurState,  this, std::placeholders::_1));
    p->RegisterCmd(mModuleName, "DumpObservers",    "Dump observers",       std::bind(&PowerManager::DebugDumpObservers, this, std::placeholders::_1));
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

void PowerManager::DebugSendPowerOn(const std::vector<std::string>& args)
{
    SprMsg msg(SIG_ID_POWER_ON);
    msg.SetI32Value(STARTUP_WARM_BOOT);
    msg.SetU32Value(WAKEUP_SOURCE_USER);
    SendMsg(SIG_ID_POWER_ON);
}

void PowerManager::DebugSendPowerOff(const std::vector<std::string>& args)
{
    SprMsg msg(SIG_ID_POWER_OFF);
    msg.SetI32Value(STANDBY_REASON_USER);
    SendMsg(msg);
}

void PowerManager::DebugDumpCurState(const std::vector<std::string>& args)
{
    SPR_LOGI("mCurLev1State    : %s\n", GetLev1String(mCurLev1State).c_str());
    SPR_LOGI("mStartupType     : %s\n", GetStartupTypeText(mStartupType).c_str());
    SPR_LOGI("mWakeupSourceType: %s\n", GetWakeupSourceTypeText(mWakeupSourceType).c_str());
    SPR_LOGI("mStandbyReason   : %s\n", GetStandbyReasonTypeText(mStandbyReason).c_str());
}

void PowerManager::DebugDumpObservers(const std::vector<std::string>& args)
{
    SPR_LOGI("                   Show  Observers Details (%02d)                                              \n", mStandbyObservers.size());
    SPR_LOGI("-----------------------------------------------------------------------------------------------\n");
    SPR_LOGI("  ID  PRIORITY               ACK                     MODULE                                    \n");
    SPR_LOGI("-----------------------------------------------------------------------------------------------\n");

    for (auto& observer : mStandbyObservers) {
        const uint32_t& id = observer.first;
        const StandbyDetail& detail = observer.second;
        SPR_LOGI("  %02d  %-21s  %-22s  %s\n", id,
            GetSprModuleBootPriorityText(detail.priority).c_str(),
            GetSprPreStandbyAckText(detail.preStandbyAck).c_str(),
            GetSprModuleIDText(id).c_str());
    }
}
