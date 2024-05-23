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
#include "PowerManager.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("PowerM", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("PowerM", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("PowerM", fmt, ##args)

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
      &PowerManager::MsgRespondPowerOnWithInit
    },

    { LEV1_POWER_STANDBY, LEV2_POWER_ANY,
      SIG_ID_POWER_ON,
      &PowerManager::MsgRespondPowerOnWithStandby
    },

    { LEV1_POWER_SLEEP, LEV2_POWER_ANY,
      SIG_ID_POWER_ON,
      &PowerManager::MsgRespondPowerOnWithSleep
    },

    // =============================================================
    // All States for SIG_ID_POWER_OFF
    // =============================================================
    { LEV1_POWER_ACTIVE, LEV2_POWER_ANY,
      SIG_ID_POWER_OFF,
      &PowerManager::MsgRespondPowerOffWithActive
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

PowerManager::PowerManager(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr)
            : SprObserver(id, name, mMsgMediatorPtr)
{
    SetLev1State(LEV1_POWER_INIT);
    SetLev2State(LEV2_POWER_ANY);
}

PowerManager::~PowerManager()
{

}

int PowerManager::ProcessMsg(const SprMsg& msg)
{
    SPR_LOGD("Recv msg: %s on %s\n", GetSigName(msg.GetMsgId()), GetLev1String(mCurLev1State).c_str());

    auto stateEntry = std::find_if(mStateTable.begin(), mStateTable.end(),
        [this, &msg](const auto& entry) {
            return ((entry.lev1State  == mCurLev1State  || entry.lev1State  == LEV1_POWER_ANY) &&
                    (entry.lev2State  == mCurLev2State  || entry.lev2State  == LEV2_POWER_ANY) &&
                    (entry.sigId      == msg.GetMsgId() || entry.sigId      == SIG_ID_ANY) );
        });

    if (stateEntry != mStateTable.end()) {
        (this->*(stateEntry->callback))(msg);
    }

    return 0;
}

std::string PowerManager::GetLev1String(EPowerLev1State lev1)
{
    #ifdef ENUM_OR_STRING
    #undef ENUM_OR_STRING
    #endif
    #define ENUM_OR_STRING(x) #x

    static std::vector<std::string> Lev1Strings = {
        POWER_LEV1_MACROS
    };

    return (Lev1Strings.size() > lev1) ? Lev1Strings[lev1] : "UNDEFINED";
}

void PowerManager::SetLev1State(EPowerLev1State state)
{
    SPR_LOGD("State changed: %s -> %s\n",
        GetLev1String(mCurLev1State).c_str(), GetLev1String(state).c_str());

    mCurLev1State = state;
}

void PowerManager::PerformBootBusiness()
{
    SetLev1State(LEV1_POWER_ACTIVE);
    BroadcastPowerEvent(SIG_ID_POWER_BOOT);
}

void PowerManager::PerformResumeBusiness()
{
    SetLev1State(LEV1_POWER_ACTIVE);
    BroadcastPowerEvent(SIG_ID_POWER_RESUME);
}

void PowerManager::PerformStandbyBusiness()
{
    SetLev1State(LEV1_POWER_STANDBY);
    BroadcastPowerEvent(SIG_ID_POWER_STANDBY);
}

void PowerManager::PerformSleepBusiness()
{
    SetLev1State(LEV1_POWER_SLEEP);
    BroadcastPowerEvent(SIG_ID_POWER_SLEEP);
}

void PowerManager::BroadcastPowerEvent(uint32_t event)
{
    SprMsg msg(event);
    NotifyAllObserver(msg);
    SPR_LOGD("Broadcast power event: %s\n", GetSigName(event));
}

void PowerManager::MsgRespondPowerOnWithInit(const SprMsg& msg)
{
    SPR_LOGD("Handle power on with default!\n");
    PerformBootBusiness();
}

void PowerManager::MsgRespondPowerOnWithStandby(const SprMsg& msg)
{
    SPR_LOGD("Handle power on with standby!\n");
    PerformResumeBusiness();
}

void PowerManager::MsgRespondPowerOnWithSleep(const SprMsg& msg)
{
    SPR_LOGD("Handle power on with sleep!\n");
    PerformResumeBusiness();
}

void PowerManager::MsgRespondPowerOffWithActive(const SprMsg& msg)
{
    SPR_LOGD("Handle power off with active!\n");
    PerformStandbyBusiness();
}

void PowerManager::MsgRespondUnexpectedMsg(const SprMsg& msg)
{
    SPR_LOGW("Ignore msg: msg = %s on %s\n",
                GetSigName(msg.GetMsgId()), GetLev1String(mCurLev1State).c_str());
}

