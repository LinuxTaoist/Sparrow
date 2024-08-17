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
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include "SprLog.h"
#include "OneNetDriver.h"
#include "EpollEventHandler.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGI(fmt, args...) LOGI("OneNetDrv", fmt, ##args)
#define SPR_LOGD(fmt, args...) LOGD("OneNetDrv", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("OneNetDrv", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("OneNetDrv", fmt, ##args)

const std::string ONENET_MQTT_HOST  = "183.230.40.39";
const int ONENET_MQTT_PORT          = 1883;

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
             : SprObserverWithMQueue(id, name)
{
    mOneNetHost = ONENET_MQTT_HOST;
    mOneNetPort = ONENET_MQTT_PORT;
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

int32_t OneNetDriver::Init()
{
    SPR_LOGD("OneNetDriver Init\n");
    return 0;
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
    auto pEpoll = EpollEventHandler::GetInstance();
    mSocketPtr = make_shared<PSocket>(AF_INET, SOCK_STREAM, 0, [&](int sock, void *arg) {
        PSocket* pClient = (PSocket*)arg;
        if (pClient == nullptr) {
            SPR_LOGE("PSocket is nullptr\n");
            return;
        }

        std::string rBuf;
        int rc = pClient->Read(sock, rBuf);
        if (rc > 0) {
            SPR_LOGD("# RECV [%d]> %d\n", sock, rBuf.size());

            std::lock_guard<std::mutex> lock(mSockMutex);
            mSockBuffer += rBuf;
        } else {
            pEpoll->DelPoll(pClient);
            SPR_LOGD("## CLOSE [%d]\n", sock);

            std::lock_guard<std::mutex> lock(mSockMutex);
            pClient->Close();
        }
    });

    int32_t rc = mSocketPtr->AsTcpClient(true, mOneNetHost, mOneNetPort);
    if (rc < 0) {
        SPR_LOGE("Failed build OneNet client! (%s)\n", strerror(errno));
        SetLev1State(LEV1_SOCKET_DISCONNECTED);
        SetLev2State(LEV2_ONENET_DISCONNECTED);
        return;
    }

    SPR_LOGI("Connect host (%s:%d) successfully!\n", mOneNetHost.c_str(), mOneNetPort);
    pEpoll->AddPoll(mSocketPtr.get());
}

/**
 * @brief Process SIG_ID_ONENET_DRV_SOCKET_DISCONNECT_ACTIVE
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondSocketDisconnectActive(const SprMsg& msg)
{
    SPR_LOGD("Recv msg id (%s), lev1: %d, lev2: %d\n", GetSigName(msg.GetMsgId()), mCurLev1State, mCurLev2State);
    mSocketPtr = nullptr;  // Smart pointer, self-destruct and close socket
    SetLev1State(LEV1_SOCKET_DISCONNECTED);
    SetLev2State(LEV2_ONENET_DISCONNECTED);
}

/**
 * @brief Process SIG_ID_ONENET_DRV_SOCKET_DISCONNECT_PASSIVE
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondSocketDisconnectPassive(const SprMsg& msg)
{
    mSocketPtr = nullptr;   // Smart pointer, self-destruct and close socket
    SetLev1State(LEV1_SOCKET_DISCONNECTED);
    SetLev2State(LEV2_ONENET_DISCONNECTED);

    // Notify disconnect state to OneNetManager
    SprMsg tmpMsg(MODULE_ONENET_MANAGER, SIG_ID_ONENET_DRV_MQTT_MSG_DISCONNECT);
    NotifyObserver(tmpMsg);
}

void OneNetDriver::MsgRespondUnexpectedState(const SprMsg& msg)
{
    SPR_LOGW("Unexpected state: msg = %s on %d %d\n",
                GetSigName(msg.GetMsgId()), mCurLev1State, mCurLev2State);
}

void OneNetDriver::MsgRespondUnexpectedMsg(const SprMsg& msg)
{
    SPR_LOGW("Unexpected msg: msg = %s on %d %d\n",
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


