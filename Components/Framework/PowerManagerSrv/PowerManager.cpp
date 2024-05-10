/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PowerManager.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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
      &PowerManager::MsgRespondPowerOnWithDefault
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
      &PowerManager::MsgRespondPowerOffWithDefault
    },

    { LEV1_POWER_ACTIVE, LEV2_POWER_ANY,
      SIG_ID_POWER_OFF,
      &PowerManager::MsgRespondPowerOffWithActive
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

    // Loop: Traverse the state table to find the matching entry
    for (const auto& it : mStateTable)
    {
        if (   (   (it.lev1State  == mCurLev1State)
                || (it.lev1State  == LEV1_POWER_ANY)
               )
            && (   (it.lev2State  == mCurLev2State)
                || (it.lev2State  == LEV2_POWER_ANY)
               )
            && (   (it.sigId      == msg.GetMsgId())
                || (it.sigId      == SIG_ID_ANY)
               )
           )
        {
            (this->*(it.callback))(msg);
            break;
        }
    }

    return 0;
}

void PowerManager::MsgRespondPowerOnWithDefault(const SprMsg& msg)
{

}

void PowerManager::MsgRespondPowerOnWithStandby(const SprMsg& msg)
{

}

void PowerManager::MsgRespondPowerOnWithSleep(const SprMsg& msg)
{

}

void PowerManager::MsgRespondPowerOffWithDefault(const SprMsg& msg)
{

}

void PowerManager::MsgRespondPowerOffWithActive(const SprMsg& msg)
{

}
