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
#include <algorithm>
#include "CommonMacros.h"
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
    { LEV1_POWER_INIT, LEV2_POWER_ANY,
      SIG_ID_POWER_OFF,
      &PowerManager::MsgRespondPowerOffWithInit
    },

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
    SPR_LOGD("Lev1: %d, Lev2: %d, msg: %s\n", mCurLev1State, mCurLev2State, GetSigName(msg.GetMsgId()));

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

void PowerManager::PerformBootBusiness()
{
    SetLev1State(LEV1_POWER_ACTIVE);
}

void PowerManager::PerformResumeBusiness()
{
    SetLev1State(LEV1_POWER_ACTIVE);
}

void PowerManager::PerformStandbyBusiness()
{
    SetLev1State(LEV1_POWER_STANDBY);
}

void PowerManager::PerformSleepBusiness()
{
    SetLev1State(LEV1_POWER_SLEEP);
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

void PowerManager::MsgRespondPowerOffWithInit(const SprMsg& msg)
{
    SPR_LOGD("Handle power off with default!\n");
    SetLev1State(LEV1_POWER_STANDBY);
}

void PowerManager::MsgRespondPowerOffWithActive(const SprMsg& msg)
{
    SPR_LOGD("Handle power off with active!\n");
    SetLev1State(LEV1_POWER_STANDBY);
}

void PowerManager::MsgRespondUnexpectedMsg(const SprMsg& msg)
{
    SPR_LOGW("IGNORE MSG: Lev1 = %d, Lev2 = %d, msg = %s\n",
                mCurLev1State, mCurLev2State, GetSigName(msg.GetMsgId()));
}

