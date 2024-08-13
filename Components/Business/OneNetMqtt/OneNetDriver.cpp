/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetDriver.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/13
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/08/13 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <algorithm>
#include "SprLog.h"
#include "OneNetDriver.h"
#include "SprMediatorIpcProxy.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("OneNetDrv", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("OneNetDrv", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("OneNetDrv", fmt, ##args)

vector <StateTransition <   EOneNetDrvLev1State,
                            EOneNetDrvLev2State,
                            ESprSigId,
                            OneNetDriver,
                            SprMsg> >
OneNetDriver::mStateTable =
{
    // =============================================================
    // All States for SIG_ID_ONENET_DRV_SOCKET_CONNECT
    // =============================================================
    { LEV1_SOCKET_IDLE, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_CONNECT,
      &OneNetDriver::MsgRespondSocketConnect
    },

    { LEV1_SOCKET_DISCONNECTED, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_CONNECT,
      &OneNetDriver::MsgRespondSocketConnect
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_CONNECT,
      &OneNetDriver::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_DRV_SOCKET_DISCONNECT_ACTIVE
    // =============================================================
    { LEV1_SOCKET_IDLE, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_DISCONNECT_ACTIVE,
      &OneNetDriver::MsgRespondSocketDisconnectActive
    },

    { LEV1_SOCKET_CONNECTING, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_DISCONNECT_ACTIVE,
      &OneNetDriver::MsgRespondSocketDisconnectActive
    },

    { LEV1_SOCKET_CONNECTED, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_DISCONNECT_ACTIVE,
      &OneNetDriver::MsgRespondSocketDisconnectActive
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_DISCONNECT_ACTIVE,
      &OneNetDriver::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_DRV_SOCKET_DISCONNECT_PASSIVE
    // =============================================================
    { LEV1_SOCKET_CONNECTING, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_DISCONNECT_PASSIVE,
      &OneNetDriver::MsgRespondSocketDisconnectPassive
    },

    { LEV1_SOCKET_CONNECTED, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_DISCONNECT_PASSIVE,
      &OneNetDriver::MsgRespondSocketDisconnectPassive
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_DISCONNECT_PASSIVE,
      &OneNetDriver::MsgRespondUnexpectedState
    },

    // =============================================================
    // Default case for handling unexpected messages,
    // mandatory to denote end of message table.
    // =============================================================
    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ANY,
      &OneNetDriver::MsgRespondUnexpectedMsg
    }
};

OneNetDriver::OneNetDriver(ModuleIDType id, const std::string& name)
             : SprObserver(id, name, std::make_shared<SprMediatorIpcProxy>())
{
    mOneNetPort = 0;
    mCurLev1State = LEV1_SOCKET_IDLE;
    mCurLev2State = LEV2_ONENET_IDLE;
}

OneNetDriver::~OneNetDriver()
{
}

OneNetDriver* OneNetDriver::GetInstance(ModuleIDType id, const std::string& name)
{
    static OneNetDriver instance(id, name);
    return &instance;
}

void OneNetDriver::SetLev1State(EOneNetDrvLev1State state)
{
    SPR_LOGD("Lev1 state changed: %d -> %d\n", mCurLev1State, state);
    mCurLev1State = state;
}

EOneNetDrvLev1State OneNetDriver::GetLev1State()
{
    return mCurLev1State;
}

void OneNetDriver::SetLev2State(EOneNetDrvLev2State state)
{
    SPR_LOGD("Lev2 state changed: %d -> %d\n", mCurLev2State, state);
    mCurLev2State = state;
}

EOneNetDrvLev2State OneNetDriver::GetLev2State()
{
    return mCurLev2State;
}

void OneNetDriver::MsgRespondSocketConnect(const SprMsg& msg)
{

}

void OneNetDriver::MsgRespondSocketDisconnectActive(const SprMsg& msg)
{

}

void OneNetDriver::MsgRespondSocketDisconnectPassive(const SprMsg& msg)
{

}

void OneNetDriver::MsgRespondUnexpectedState(const SprMsg& msg)
{

}

void OneNetDriver::MsgRespondUnexpectedMsg(const SprMsg& msg)
{
    SPR_LOGW("Ignore msg: msg = %s on %d %d\n",
                GetSigName(msg.GetMsgId()), mCurLev1State, mCurLev2State);
}

int32_t OneNetDriver::ProcessMsg(const SprMsg& msg)
{
    SPR_LOGD("Recv msg: %s on Lev1 %d Lev2 %d\n", GetSigName(msg.GetMsgId()), mCurLev1State, mCurLev2State);

    auto stateEntry = std::find_if(mStateTable.begin(), mStateTable.end(),
        [this, &msg](const StateTransitionType& entry) {
            return ((entry.lev1State  == mCurLev1State  || entry.lev1State  == LEV1_SOCKET_ANY) &&
                    (entry.lev2State  == mCurLev2State  || entry.lev2State  == LEV2_ONENET_ANY) &&
                    (entry.sigId      == msg.GetMsgId() || entry.sigId      == SIG_ID_ANY) );
        });

    if (stateEntry != mStateTable.end()) {
        (this->*(stateEntry->callback))(msg);
    }

    return 0;
}


