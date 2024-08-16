/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetManager.cpp
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
#include "OneNetManager.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("OneNetMgr", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("OneNetMgr", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("OneNetMgr", fmt, ##args)

vector <StateTransition <   EOneNetMgrLev1State,
                            EOneNetMgrLev2State,
                            ESprSigId,
                            OneNetManager,
                            SprMsg> >
OneNetManager::mStateTable =
{
    // =============================================================
    // All States for SIG_ID_ONENET_DRV_SOCKET_CONNECT
    // =============================================================
    { LEV1_ONENET_MGR_CONNECTING, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_CONNACK,
      &OneNetManager::MsgRespondMqttConnAck
    },

    { LEV1_ONENET_MGR_DISCONNECTED, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_CONNACK,
      &OneNetManager::MsgRespondMqttConnAck
    },

    { LEV1_ONENET_MGR_ANY, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_CONNACK,
      &OneNetManager::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_DRV_MQTT_MSG_DISCONNECT
    // =============================================================
    { LEV1_ONENET_MGR_CONNECTING, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_DISCONNECT,
      &OneNetManager::MsgRespondMqttDisconnect
    },

    { LEV1_ONENET_MGR_CONNECTED, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_DISCONNECT,
      &OneNetManager::MsgRespondMqttDisconnect
    },

    { LEV1_ONENET_MGR_ANY, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_DISCONNECT,
      &OneNetManager::MsgRespondUnexpectedState
    },

    // =============================================================
    // Default case for handling unexpected messages,
    // mandatory to denote end of message table.
    // =============================================================
    { LEV1_ONENET_MGR_ANY, LEV2_ONENET_MGR_ANY,
      SIG_ID_ANY,
      &OneNetManager::MsgRespondUnexpectedMsg
    }
};

OneNetManager::OneNetManager(ModuleIDType id, const std::string& name)
  : SprObserverWithMQueue(id, name)
{
    mCurLev1State = LEV1_ONENET_MGR_IDLE;
    mCurLev2State = LEV2_ONENET_MGR_ANY;
}

OneNetManager::~OneNetManager()
{
}

OneNetManager* OneNetManager::GetInstance(ModuleIDType id, const std::string& name)
{
    static OneNetManager instance(id, name);
    return &instance;
}

int32_t OneNetManager::Init()
{
    SPR_LOGD("OneNetManager Init\n");

    // debug
    SprMsg msg(SIG_ID_ONENET_DRV_SOCKET_CONNECT);
    int ret = NotifyObserver(MODULE_ONENET_DRIVER, msg);
    SPR_LOGD("NotifyObserver ret %d\n", ret);
    return 0;
}

void OneNetManager::SetLev1State(EOneNetMgrLev1State state)
{
    mCurLev1State = state;
    SPR_LOGD("Lev2 state changed: %d -> %d\n", mCurLev1State, state);
}

EOneNetMgrLev1State OneNetManager::GetLev1State()
{
    return mCurLev1State;
}

void OneNetManager::SetLev2State(EOneNetMgrLev2State state)
{
    mCurLev2State = state;
    SPR_LOGD("Lev2 state changed: %d -> %d\n", mCurLev2State, state);
}

EOneNetMgrLev2State OneNetManager::GetLev2State()
{
    return mCurLev2State;
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_CONNECT
 *
 * @param[in] msg
 * @return none
 */
void OneNetManager::MsgRespondMqttConnect(const SprMsg& msg)
{

}

void OneNetManager::MsgRespondMqttConnAck(const SprMsg& msg)
{
    SPR_LOGD("Lev1 state changed: %d -> %d\n", mCurLev1State, LEV1_ONENET_MGR_CONNECTED);
    SetLev1State(LEV1_ONENET_MGR_CONNECTED);
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_DISCONNECT
 *
 * @param[in] msg
 * @return none
 */
void OneNetManager::MsgRespondMqttDisconnect(const SprMsg& msg)
{
    SPR_LOGD("Lev1 state changed: %d -> %d\n", mCurLev1State, LEV1_ONENET_MGR_DISCONNECTED);
    SetLev1State(LEV1_ONENET_MGR_DISCONNECTED);
}

/**
 * @brief Process
 *
 * @param[in] msg
 * @return none
 */
void OneNetManager::MsgRespondUnexpectedState(const SprMsg& msg)
{

}

/**
 * @brief Process
 *
 * @param[in] msg
 * @return none
 */
void OneNetManager::MsgRespondUnexpectedMsg(const SprMsg& msg)
{

}

int32_t OneNetManager::ProcessMsg(const SprMsg& msg)
{
    SPR_LOGD("Recv msg: %s on Lev1 %d Lev2 %d\n", GetSigName(msg.GetMsgId()), mCurLev1State, mCurLev2State);

    auto stateEntry = std::find_if(mStateTable.begin(), mStateTable.end(),
        [this, &msg](const StateTransitionType& entry) {
            return ((entry.lev1State  == mCurLev1State  || entry.lev1State  == LEV1_ONENET_MGR_ANY) &&
                    (entry.lev2State  == mCurLev2State  || entry.lev2State  == LEV2_ONENET_MGR_ANY) &&
                    (entry.sigId      == msg.GetMsgId() || entry.sigId      == SIG_ID_ANY) );
        });

    if (stateEntry != mStateTable.end()) {
        (this->*(stateEntry->callback))(msg);
    }

    return 0;
}
