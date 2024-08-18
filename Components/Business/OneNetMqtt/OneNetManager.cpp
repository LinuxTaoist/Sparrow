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
    // All States for SIG_ID_ONENET_DRV_MQTT_MSG_CONNACK
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
    return 0;
}

void OneNetManager::SetLev1State(EOneNetMgrLev1State state)
{
    if (mCurLev1State == state) {
        return;
    }

    mCurLev1State = state;
    SPR_LOGD("Lev1 state changed: %s -> %s\n", GetLev1StateString(mCurLev1State), GetLev1StateString(state));
}

EOneNetMgrLev1State OneNetManager::GetLev1State()
{
    return mCurLev1State;
}

const char* OneNetManager::GetLev1StateString(EOneNetMgrLev1State state)
{
    #ifdef ENUM_OR_STRING
    #undef ENUM_OR_STRING
    #endif
    #define ENUM_OR_STRING(x) #x

    static std::vector<std::string> Lev2Strings = {
        ONENET_MGR_LEV1_MACROS
    };

    return (Lev2Strings.size() > state) ? Lev2Strings[state].c_str() : "UNDEFINED";
}

void OneNetManager::SetLev2State(EOneNetMgrLev2State state)
{
    if (mCurLev2State == state) {
        return;
    }

    mCurLev2State = state;
    SPR_LOGD("Lev2 state changed: %s -> %s\n", GetLev2StateString(mCurLev2State), GetLev2StateString(state));
}

EOneNetMgrLev2State OneNetManager::GetLev2State()
{
    return mCurLev2State;
}

const char* OneNetManager::GetLev2StateString(EOneNetMgrLev2State state)
{
    #ifdef ENUM_OR_STRING
    #undef ENUM_OR_STRING
    #endif
    #define ENUM_OR_STRING(x) #x

    static std::vector<std::string> Lev2Strings = {
        ONENET_MGR_LEV2_MACROS
    };

    return (Lev2Strings.size() > state) ? Lev2Strings[state].c_str() : "UNDEFINED";
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
    SPR_LOGW("Unexpected msg: msg = %s on <%s : %s>\n",
        GetSigName(msg.GetMsgId()), GetLev1StateString(mCurLev1State), GetLev2StateString(mCurLev2State));
}

/**
 * @brief Process
 *
 * @param[in] msg
 * @return none
 */
void OneNetManager::MsgRespondUnexpectedMsg(const SprMsg& msg)
{
    SPR_LOGW("Unexpected state: msg = %s on <%s : %s>\n",
        GetSigName(msg.GetMsgId()), GetLev1StateString(mCurLev1State), GetLev2StateString(mCurLev2State));
}

int32_t OneNetManager::ProcessMsg(const SprMsg& msg)
{
    SPR_LOGD("Recv msg: %s on <%s : %s>\n", GetSigName(msg.GetMsgId()),
              GetLev1StateString(mCurLev1State), GetLev2StateString(mCurLev2State));

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
