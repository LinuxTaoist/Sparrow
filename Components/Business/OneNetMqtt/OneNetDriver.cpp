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
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include "SprLog.h"
#include "OneNetDriver.h"
#include "OneNetCommon.h"
#include "SprEpollSchedule.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGI(fmt, args...) LOGI("OneNetDrv", fmt, ##args)
#define SPR_LOGD(fmt, args...) LOGD("OneNetDrv", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("OneNetDrv", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("OneNetDrv", fmt, ##args)

const std::string ONENET_MQTT_HOST  = "183.230.40.96";
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
    // All States for SIG_ID_ONENET_DRV_SOCKET_CONNECT_SUCCESS
    // =============================================================
    { LEV1_SOCKET_CONNECTING, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_CONNECT_SUCCESS,
      &OneNetDriver::MsgRespondSocketConnectSuccess
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_CONNECT_SUCCESS,
      &OneNetDriver::MsgRespondUnexpectedState
    },
    // =============================================================
    // All States for SIG_ID_ONENET_DRV_SOCKET_CONNECT_FAIL
    // =============================================================
    { LEV1_SOCKET_CONNECTING, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_CONNECT_FAIL,
      &OneNetDriver::MsgRespondSocketConnectFail
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_CONNECT_FAIL,
      &OneNetDriver::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_DRV_SOCKET_RECONNECT
    // =============================================================
    { LEV1_SOCKET_IDLE, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_RECONNECT,
      &OneNetDriver::MsgRespondSocketReconnect
    },

    { LEV1_SOCKET_DISCONNECTED, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_RECONNECT,
      &OneNetDriver::MsgRespondSocketReconnect
    },

    { LEV1_SOCKET_CONNECTED, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_RECONNECT,
      &OneNetDriver::MsgRespondSocketReconnect
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_RECONNECT,
      &OneNetDriver::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_DRV_SOCKET_RECONNECT_TIMER_EVENT
    // =============================================================
    { LEV1_SOCKET_IDLE, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_RECONNECT_TIMER_EVENT,
      &OneNetDriver::MsgRespondSocketReconnectTimerEvent
    },

    { LEV1_SOCKET_DISCONNECTED, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_RECONNECT_TIMER_EVENT,
      &OneNetDriver::MsgRespondSocketReconnectTimerEvent
    },

    { LEV1_SOCKET_CONNECTED, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_RECONNECT_TIMER_EVENT,
      &OneNetDriver::MsgRespondSocketReconnectTimerEvent
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_SOCKET_RECONNECT_TIMER_EVENT,
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
    // All States for SIG_ID_ONENET_DRV_MQTT_MSG_CONNECT
    // =============================================================
    { LEV1_SOCKET_ANY, LEV2_ONENET_IDLE,
      SIG_ID_ONENET_DRV_MQTT_MSG_CONNECT,
      &OneNetDriver::MsgRespondMqttMsgConnect
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_CONNECTING,
      SIG_ID_ONENET_DRV_MQTT_MSG_CONNECT,
      &OneNetDriver::MsgRespondMqttMsgConnect
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_DISCONNECTED,
      SIG_ID_ONENET_DRV_MQTT_MSG_CONNECT,
      &OneNetDriver::MsgRespondMqttMsgConnect
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_CONNECT,
      &OneNetDriver::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_DRV_MQTT_MSG_CONNACK
    // =============================================================
    { LEV1_SOCKET_CONNECTED, LEV2_ONENET_CONNECTING,
      SIG_ID_ONENET_DRV_MQTT_MSG_CONNACK,
      &OneNetDriver::MsgRespondMqttMsgConnack
    },

    { LEV1_SOCKET_CONNECTED, LEV2_ONENET_DISCONNECTED,
      SIG_ID_ONENET_DRV_MQTT_MSG_CONNACK,
      &OneNetDriver::MsgRespondMqttMsgConnack
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_CONNACK,
      &OneNetDriver::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_DRV_MQTT_MSG_PUBLISH
    // =============================================================
    { LEV1_SOCKET_CONNECTED, LEV2_ONENET_CONNECTED,
      SIG_ID_ONENET_DRV_MQTT_MSG_PUBLISH,
      &OneNetDriver::MsgRespondMqttMsgPublish
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_PUBLISH,
      &OneNetDriver::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_DRV_MQTT_MSG_SUBSCRIBE
    // =============================================================
    { LEV1_SOCKET_CONNECTED, LEV2_ONENET_CONNECTED,
      SIG_ID_ONENET_DRV_MQTT_MSG_SUBSCRIBE,
      &OneNetDriver::MsgRespondMqttMsgSubscribe
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_SUBSCRIBE,
      &OneNetDriver::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_DRV_MQTT_MSG_PINGREQ
    // =============================================================
    { LEV1_SOCKET_CONNECTED, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_PINGREQ,
      &OneNetDriver::MsgRespondMqttMsgPingreq
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_PINGREQ,
      &OneNetDriver::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_DRV_MQTT_MSG_PINGRESP
    // =============================================================
    { LEV1_SOCKET_CONNECTED, LEV2_ONENET_CONNECTED,
      SIG_ID_ONENET_DRV_MQTT_MSG_PINGRESP,
      &OneNetDriver::MsgRespondMqttMsgPingresq
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_PINGRESP,
      &OneNetDriver::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_DRV_MQTT_MSG_DISCONNECT
    // =============================================================
    { LEV1_SOCKET_CONNECTED, LEV2_ONENET_CONNECTED,
      SIG_ID_ONENET_DRV_MQTT_MSG_DISCONNECT,
      &OneNetDriver::MsgRespondMqttMsgDisconnect
    },

    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_DISCONNECT,
      &OneNetDriver::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_MGR_DEBUG_ENABLE
    // =============================================================
    { LEV1_SOCKET_ANY, LEV2_ONENET_ANY,
      SIG_ID_ONENET_MGR_DEBUG_ENABLE,
      &OneNetDriver::MsgRespondDebugEnable
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
    mDebugEnable = false;
    mEnableReconTimer = false;
    mUnixPipeFd[0] = -1;
    mUnixPipeFd[1] = -1;
    mOneNetHost = ONENET_MQTT_HOST;
    mOneNetPort = ONENET_MQTT_PORT;
    mCurLev1State = LEV1_SOCKET_IDLE;
    mCurLev2State = LEV2_ONENET_IDLE;
    mOneSocketPtr = nullptr;
    mSendPIPEPtr = nullptr;
    mRecvPIPEPtr = nullptr;
}

OneNetDriver::~OneNetDriver()
{
    if (mUnixPipeFd[0] != -1) {
        close(mUnixPipeFd[0]);
        mUnixPipeFd[0] = -1;
    }

    if (mUnixPipeFd[1] != -1) {
        close(mUnixPipeFd[1]);
        mUnixPipeFd[1] = -1;
    }
}

int32_t OneNetDriver::Init()
{
    SPR_LOGD("OneNetDriver Init\n");
    InitUnixPIPE();
    return 0;
}

int32_t OneNetDriver::InitUnixPIPE()
{
    SPR_LOGD("Init Unix PIPE\n");
    int32_t ret = socketpair(AF_UNIX, SOCK_STREAM, 0, mUnixPipeFd);
    if (ret == -1) {
        SPR_LOGE("socketpair failed: %s\n", strerror(errno));
        return -1;
    }

    // 将待发的mqtt字节流通过mSendPIPEPtr写入管道pipe[1]中缓存
    mSendPIPEPtr = mSendPIPEPtr ? mSendPIPEPtr : new (std::nothrow) SprObserverWithSocket(mUnixPipeFd[1]);
    CHECK_ONENET_POINTER(mSendPIPEPtr, -1);
    mSendPIPEPtr->AsUnixStreamClient();

    // 读取管道pipe[0]中缓存的mqtt字节流
    mRecvPIPEPtr = mRecvPIPEPtr ? mRecvPIPEPtr : new (std::nothrow) SprObserverWithSocket(mUnixPipeFd[0], [&](int sock, void *arg) {
        PSocket* pUnixPIPE0 = reinterpret_cast<PSocket*>(arg);
        CHECK_ONENET_POINTER_NONRET(pUnixPIPE0);
        CHECK_ONENET_POINTER_NONRET(mOneSocketPtr);

        std::string rBuf;
        int32_t len = pUnixPIPE0->Read(sock, rBuf);
        if (len < 0) {
            SPR_LOGE("Read unix pipe0 failed! %s\n", strerror(errno));
            return;
        }

        // 读取出管道缓存mqtt字节流，通过socket发送到OneNet
        SPR_LOGD("## SEND [%d]> %d\n", sock, rBuf.size());
        DumpSocketBytesWithAscall(rBuf);
        len = mOneSocketPtr->Write(mOneSocketPtr->GetEpollFd(), rBuf);
        if (len < 0) {
            SPR_LOGE("Write socket failed! %s\n", strerror(errno));
        }

        // 主动断开时，先发送断开消息，再关闭socket
        if (mCurLev1State == LEV1_SOCKET_DISCONNECTED &&
            mCurLev2State == LEV2_ONENET_DISCONNECTED) {
            mOneSocketPtr->Close();
            SPR_LOGD("Close socket\n");
        }
    });

    CHECK_ONENET_POINTER(mRecvPIPEPtr, -1);
    mRecvPIPEPtr->AsUnixStreamClient();
    mRecvPIPEPtr->InitFramework();
    return ret;
}

void OneNetDriver::SetLev1State(EOneNetDrvLev1State state)
{
    if (mCurLev1State == state) {
        return;
    }

    SPR_LOGD("Lev1 state changed: %s -> %s\n", GetLev1StateString(mCurLev1State), GetLev1StateString(state));
    mCurLev1State = state;
}

const char* OneNetDriver::GetLev1StateString(EOneNetDrvLev1State state)
{
    #ifdef ENUM_OR_STRING
    #undef ENUM_OR_STRING
    #endif
    #define ENUM_OR_STRING(x) #x

    static std::vector<std::string> Lev1Strings = {
        ONENET_DRV_LEV1_MACROS
    };

    return (Lev1Strings.size() > state) ? Lev1Strings[state].c_str() : "UNDEFINED";
}

EOneNetDrvLev1State OneNetDriver::GetLev1State()
{
    return mCurLev1State;
}

void OneNetDriver::SetLev2State(EOneNetDrvLev2State state)
{
    if (mCurLev2State == state) {
        return;
    }

    SPR_LOGD("Lev2 state changed: %s -> %s\n", GetLev2StateString(mCurLev2State), GetLev2StateString(state));
    mCurLev2State = state;
}

const char* OneNetDriver::GetLev2StateString(EOneNetDrvLev2State state)
{
    #ifdef ENUM_OR_STRING
    #undef ENUM_OR_STRING
    #endif
    #define ENUM_OR_STRING(x) #x

    static std::vector<std::string> Lev2Strings = {
        ONENET_DRV_LEV2_MACROS
    };

    return (Lev2Strings.size() > state) ? Lev2Strings[state].c_str() : "UNDEFINED";
}

EOneNetDrvLev2State OneNetDriver::GetLev2State()
{
    return mCurLev2State;
}

int32_t OneNetDriver::DumpSocketBytes(const std::string& tag, const std::string& bytes)
{
    if (!mDebugEnable) {
        return 0;
    }

    std::stringstream hexBytes;
    hexBytes << std::hex << std::setfill('0');

    for (const auto& it : bytes) {
        hexBytes << std::setw(2) << uint32_t((uint8_t)(it)) << " ";
    }

    SPR_LOGD("[%s] %s\n", tag.c_str(), hexBytes.str().c_str());
    return 0;
}

int32_t OneNetDriver::DumpSocketBytesWithAscall(const std::string& bytes)
{
    if (!mDebugEnable) {
        return 0;
    }

    const int32_t BYTES_PER_LINE = 16; // Number of bytes per line in the dump.
    int32_t length = bytes.length();

    for (int32_t i = 0; i < length; i += BYTES_PER_LINE) {
        std::stringstream posBytes, hexBytes, ascallBytes;
        int32_t startByte = i;
        int32_t endByte = std::min(startByte + BYTES_PER_LINE, length);

        // address bytes
        posBytes << std::hex << std::setw(8) << std::setfill('0') << startByte << "  ";

        ascallBytes << " |";
        hexBytes << std::hex << std::setfill('0');
        for (int32_t j = 0; j < BYTES_PER_LINE; ++j) {
            if (startByte + j < endByte) {
                hexBytes << std::setw(2) << uint32_t(uint8_t(bytes[startByte + j])) << " ";

                char ch = bytes[startByte + j];
                char chAscall = (ch >= 32 && ch <= 126) ? ch : '.';
                ascallBytes << chAscall;
            } else {
                hexBytes << "   ";
            }
        }

        ascallBytes << "|";
        SPR_LOGD("%s%s%s\n", posBytes.str().c_str(), hexBytes.str().c_str(), ascallBytes.str().c_str());
    }

    return 0;
}

/**
 * @brief Process SIG_ID_ONENET_DRV_SOCKET_CONNECT
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondSocketConnect(const SprMsg& msg)
{
    if (!mOneSocketPtr) {
        delete mOneSocketPtr;
        mOneSocketPtr = nullptr;
    }

    mOneSocketPtr = new (std::nothrow) SprObserverWithSocket(AF_INET, SOCK_STREAM, 0,[&](int sock, void *arg) {
        PSocket* pSocket = reinterpret_cast<PSocket*>(arg);
        if (pSocket == nullptr) {
            SPR_LOGE("PSocket is nullptr\n");
            return;
        }

        std::string rBuf;
        int rc = pSocket->Read(sock, rBuf);
        if (rc > 0) {
            SPR_LOGD("## RECV [%d]> %d\n", sock, rBuf.size());
            DumpSocketBytesWithAscall(rBuf);
            DispatchMqttBytes(rBuf);
        } else {
            SPR_LOGD("## CLOSE [%d]\n", sock);
            pSocket->Close();

            // Send to self socket disconnect passive
            SprMsg tmpMsg(SIG_ID_ONENET_DRV_SOCKET_DISCONNECT_PASSIVE);
            SendMsg(tmpMsg);
        }
    });

    // Update state to connecting
    SetLev1State(LEV1_SOCKET_CONNECTING);

    CHECK_ONENET_POINTER_NONRET(mOneSocketPtr);
    int32_t rc = mOneSocketPtr->AsTcpClient(true, mOneNetHost, mOneNetPort);
    if (rc < 0) {
        SPR_LOGE("Failed build OneNet client! (%s)\n", strerror(errno));
        SprMsg disConMsg(SIG_ID_ONENET_DRV_SOCKET_CONNECT_FAIL);
        SendMsg(disConMsg);
        return;
    }

    mOneSocketPtr->InitFramework();
    SprMsg conMsg(SIG_ID_ONENET_DRV_SOCKET_CONNECT_SUCCESS);
    SendMsg(conMsg);
    SPR_LOGI("Connect host (%s:%d) successfully!\n", mOneNetHost.c_str(), mOneNetPort);
}

/**
 * @brief Process SIG_ID_ONENET_DRV_SOCKET_CONNECT_SUCCESS
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondSocketConnectSuccess(const SprMsg& msg)
{
    SetLev1State(LEV1_SOCKET_CONNECTED);

    if (mCurLev2State == LEV2_ONENET_CONNECTING) {
        SPR_LOGI("Socket connected, please resend connect with device infomation!\n");
        SprMsg reConMsg(SIG_ID_ONENET_MGR_REACTIVE_CUR_DEVICE_CONNECT);
        NotifyObserver(MODULE_ONENET_MANAGER, reConMsg);
    }

    SPR_LOGI("Connect OneNet socket successfully!\n");
}

/**
 * @brief Process SIG_ID_ONENET_DRV_SOCKET_CONNECT_FAIL
 *
 * @param msg
 */
void OneNetDriver::MsgRespondSocketConnectFail(const SprMsg& msg)
{
    SetLev1State(LEV1_SOCKET_DISCONNECTED);
    SetLev2State(LEV2_ONENET_DISCONNECTED);

    SprMsg reConMsg(SIG_ID_ONENET_DRV_SOCKET_RECONNECT);
    SendMsg(reConMsg);
    SPR_LOGI("Connect OneNet socket failed!\n");
}

/**
 * @brief Process SIG_ID_ONENET_DRV_SOCKET_RECONNECT
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondSocketReconnect(const SprMsg& msg)
{
    // book a long-term timer for reconnect socket
    const int32_t internalInMSec = 5000;  // 5 seconds
    if (!mEnableReconTimer) {
        SPR_LOGD("Start reconnect timer. internalInMSec = %d\n", internalInMSec);
        RegisterTimer(internalInMSec, internalInMSec, SIG_ID_ONENET_DRV_SOCKET_RECONNECT_TIMER_EVENT, 0);
        mEnableReconTimer = true;
    }
}

/**
 * @brief Process SIG_ID_ONENET_DRV_SOCKET_RECONNECT_TIMER_EVENT
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondSocketReconnectTimerEvent(const SprMsg& msg)
{
    static int32_t cnt = 0;
    if (LEV1_SOCKET_CONNECTED == mCurLev1State) {
        cnt = 0;
        SPR_LOGD("Already connected, stop reconnect timer. en = %d\n", mEnableReconTimer);

        if (mEnableReconTimer) {
            UnregisterTimer(SIG_ID_ONENET_DRV_SOCKET_RECONNECT_TIMER_EVENT);
            mEnableReconTimer = false;
        }
        return;
    }

    SendMsg(SIG_ID_ONENET_DRV_SOCKET_CONNECT);
    SPR_LOGD("Receive reconnet timer. connect socket cnt = %d, mEnableReconTimer = %d\n", ++cnt, mEnableReconTimer);
}

/**
 * @brief Process SIG_ID_ONENET_DRV_SOCKET_DISCONNECT_ACTIVE
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondSocketDisconnectActive(const SprMsg& msg)
{
    // close socket after send disconnect msg
    // mOneSocketPtr->Close();  // Smart pointer, self-destruct and close socket
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
    if (LEV1_SOCKET_CONNECTED == mCurLev1State) {
        // Notify mqtt device disconnect state to OneNetManager
        SprMsg msg1(MODULE_ONENET_MANAGER, SIG_ID_ONENET_DRV_MQTT_MSG_DISCONNECT);
        NotifyObserver(msg1);
    }

    // close socket on client side, reconnect socket and OneNet
    mOneSocketPtr->Close();
    SetLev1State(LEV1_SOCKET_DISCONNECTED);
    SetLev2State(LEV2_ONENET_CONNECTING);

    // reconnect socket
    SprMsg msg2(SIG_ID_ONENET_DRV_SOCKET_RECONNECT);
    SendMsg(msg2);
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_CONNECT
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondMqttMsgConnect(const SprMsg& msg)
{
    if (mCurLev1State != LEV1_SOCKET_CONNECTED) {
        SPR_LOGW("Socket not ready, try connect...\n");
        SetLev2State(LEV2_ONENET_CONNECTING);
        SendMsg(SIG_ID_ONENET_DRV_SOCKET_CONNECT);
        return;
    }

    int32_t keepAlive = msg.GetU32Value();
    std::string payload = msg.GetString();
    int32_t ret = SendMqttConnect(keepAlive, payload);
    if (ret < 0) {
        SPR_LOGE("Send mqtt connect msg failed\n");
    }
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_CONNACK
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondMqttMsgConnack(const SprMsg& msg)
{
    uint8_t conResult = msg.GetU8Value();
    if (conResult == 0) {
        SetLev2State(LEV2_ONENET_CONNECTED);
    }

    SprMsg copyMsg(msg);
    NotifyObserver(MODULE_ONENET_MANAGER, copyMsg);
    SPR_LOGD("Connect OneNet result = %d\n", conResult);
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_PUBLISH
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondMqttMsgPublish(const SprMsg& msg)
{
    std::shared_ptr<SOneNetPublishParam> pParam = msg.GetDatas<SOneNetPublishParam>();
    std::string topic = pParam->topic;
    std::string payload = pParam->payload;
    int32_t ret = SendMqttPublish(pParam->flags, pParam->identifier, topic, payload);
    if (ret < 0) {
        SPR_LOGE("Send mqtt pingreq msg failed\n");
    }

    SPR_LOGD("Send publish msg, 0x%x topic = %s, payload = %s\n", pParam->identifier, pParam->topic, pParam->payload);
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_SUBSCRIBE
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondMqttMsgSubscribe(const SprMsg& msg)
{
    uint16_t identifer = msg.GetU16Value();
    std::string payload = msg.GetString();
    int32_t ret = SendMqttSubscribe(identifer, payload);
    if (ret < 0) {
        SPR_LOGE("send subscribe failed\n");
    }
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_PINGREQ
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondMqttMsgPingreq(const SprMsg& msg)
{
    int32_t ret = SendMqttPingReq();
    if (ret < 0) {
        SPR_LOGE("Send mqtt pingreq msg failed\n");
    }
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_PINGRESP
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondMqttMsgPingresq(const SprMsg& msg)
{
    SPR_LOGD("Recv mqtt ping response!\n");
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_DISCONNECT
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondMqttMsgDisconnect(const SprMsg& msg)
{
    int32_t ret = SendMqttDisconnect();
    if (ret < 0) {
        SPR_LOGE("Send mqtt disconnect msg failed\n");
    }

    if (mCurLev1State == LEV1_SOCKET_CONNECTED) {
        SprMsg sockDisMsg(SIG_ID_ONENET_DRV_SOCKET_DISCONNECT_ACTIVE);
        SendMsg(sockDisMsg);
    }
}

void OneNetDriver::MsgRespondDebugEnable(const SprMsg& msg)
{
    mDebugEnable = msg.GetBoolValue();
    SPR_LOGD("mDebugEnable = %d\n", mDebugEnable);
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_ANY
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondUnexpectedState(const SprMsg& msg)
{
    SPR_LOGW("Unexpected state: msg = %s on %s : %s\n",
                GetSigName(msg.GetMsgId()), GetLev1StateString(mCurLev1State), GetLev2StateString(mCurLev2State));
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_ANY
 *
 * @param[in] msg
 * @return none
 */
void OneNetDriver::MsgRespondUnexpectedMsg(const SprMsg& msg)
{
    SPR_LOGW("Unexpected msg: msg = %s on %s : %s\n",
                GetSigName(msg.GetMsgId()), GetLev1StateString(mCurLev1State), GetLev2StateString(mCurLev2State));
}

int32_t OneNetDriver::SendMqttConnect(int32_t keepAliveInSec, const std::string& payload)
{
    std::string protocol = "MQTT";
    uint16_t keepalive = keepAliveInSec;
    MqttConnect conMsg(protocol, 0x04, 0xC2, keepalive);
    conMsg.SetPayload(payload);

    std::string bytes;
    conMsg.Encode(bytes);
    return SendMqttBytes(bytes);
}

int32_t OneNetDriver::SendMqttPublish(uint8_t flags, uint16_t identifier, std::string& topic, std::string& payload)
{
    MqttPublish mqttMsg(flags, identifier, topic, payload);
    std::string bytes;
    mqttMsg.Encode(bytes);
    return SendMqttBytes(bytes);
}

int32_t OneNetDriver::SendMqttPubAck(uint16_t cmd)
{
    return 0;
}

int32_t OneNetDriver::SendMqttPubRec(uint16_t cmd)
{
    return 0;
}

int32_t OneNetDriver::SendMqttPubRel(uint16_t cmd)
{
    return 0;
}

int32_t OneNetDriver::SendMqttPubComp(uint16_t cmd)
{
    return 0;
}

int32_t OneNetDriver::SendMqttSubscribe(uint16_t identifier, std::string& topics)
{
    SPR_LOGD("Send mqtt subscribe!\n");
    MqttSubscribe mqttMsg(identifier, topics);
    std::string bytes;
    mqttMsg.Encode(bytes);
    return SendMqttBytes(bytes);
}

int32_t OneNetDriver::SendMqttSubAck(uint16_t cmd)
{
    return 0;
}

int32_t OneNetDriver::SendMqttUnsubscribe(uint16_t cmd)
{
    return 0;
}

int32_t OneNetDriver::SendMqttUnsubAck(uint16_t cmd)
{
    return 0;
}

int32_t OneNetDriver::SendMqttPingReq()
{
    SPR_LOGD("Send mqtt ping request!\n");
    MqttPingreq pingreqMsg;
    std::string bytes;
    pingreqMsg.Encode(bytes);
    return SendMqttBytes(bytes);
}

int32_t OneNetDriver::SendMqttPingResp()
{
    return 0;
}

int32_t OneNetDriver::SendMqttDisconnect()
{
    SPR_LOGD("Send mqtt disconnect!\n");
    MqttDisconnect disconMsg;
    std::string bytes;
    disconMsg.Encode(bytes);
    return SendMqttBytes(bytes);
}

int32_t OneNetDriver::SendMqttBytes(const std::string& bytes)
{
    if (!mSendPIPEPtr) {
        SPR_LOGE("Send PIPE is null\n");
        return -1;
    }

    int32_t len = mSendPIPEPtr->Write(mSendPIPEPtr->GetEpollFd(), bytes);
    if (len < 0) {
        SPR_LOGE("Send PIPE failed! (%s)\n", strerror(errno));
    }

    // SPR_LOGD("PIPE %d Send %d bytes\n", mSendPIPEPtr->GetEpollFd(), len);
    return len;
}

int32_t OneNetDriver::DispatchMqttBytes(const std::string& bytes)
{
    if (bytes.size() < 2) {
        SPR_LOGE("Invalid mqtt bytes\n");
        return -1;
    }

    int32_t ret = -1;
    uint8_t msgId = (bytes[0] & 0xF0) >> 4;
    switch (msgId) {
        case MQTT_MSG_CONNACK: {
            ret = HandleMqttConnack(bytes);
            break;
        }
        case MQTT_MSG_PUBLISH: {
            ret = HandleMqttPublish(bytes);
            break;
        }
        case MQTT_MSG_PUBACK: {
            ret = HandleMqttPubAck(bytes);
            break;
        }
        case MQTT_MSG_SUBACK: {
            ret = HandleMqttSubAck(bytes);
            break;
        }
        case MQTT_MSG_PINGRESP: {
            ret = HandleMqttPingResp(bytes);
            break;
        }
        default: {
            SPR_LOGE("Invalid mqtt msgId %d\n", msgId);
            break;
        }
    }

    return ret;
}

int32_t OneNetDriver::HandleMqttConnack(const std::string& bytes)
{
    MqttConnack mqttCmd;
    int32_t ret = mqttCmd.Decode(bytes);
    CHECK_ONENET_RET_VALIDITY(ret);

    SprMsg msg(SIG_ID_ONENET_DRV_MQTT_MSG_CONNACK);
    msg.SetU8Value(mqttCmd.GetReturnCode());
    SendMsg(msg);
    return ret;
}

int32_t OneNetDriver::HandleMqttPublish(const std::string& bytes)
{
    MqttPublish mqttCmd;
    int32_t ret = mqttCmd.Decode(bytes);
    CHECK_ONENET_RET_VALIDITY(ret);

    std::string payload, topic;
    mqttCmd.GetPayload(payload);
    topic = mqttCmd.GetTopic();

    SOneNetPublishParam publishParam {};
    strncpy(publishParam.topic, topic.c_str(), sizeof(publishParam.topic) - 1);
    strncpy(publishParam.payload, payload.c_str(), sizeof(publishParam.payload) - 1);
    publishParam.topic[sizeof(publishParam.topic) - 1] = '\0';
    publishParam.payload[sizeof(publishParam.payload) - 1] = '\0';

    SPR_LOGD("Recv mqtt publish! topic: %s payload: %s\n", topic.c_str(), payload.c_str());
    return 0;
}

int32_t OneNetDriver::HandleMqttPubAck(const std::string& bytes)
{
    return 0;
}

int32_t OneNetDriver::HandleMqttPubRec(const std::string& bytes)
{
    return 0;
}

int32_t OneNetDriver::HandleMqttPubRel(const std::string& bytes)
{
    return 0;
}

int32_t OneNetDriver::HandleMqttPubComp(const std::string& bytes)
{
    return 0;
}

int32_t OneNetDriver::HandleMqttSubscribe(const std::string& bytes)
{
    return 0;
}

int32_t OneNetDriver::HandleMqttSubAck(const std::string& bytes)
{
    Suback mqttCmd;
    int32_t ret = mqttCmd.Decode(bytes);
    CHECK_ONENET_RET_VALIDITY(ret);

    SprMsg msg(SIG_ID_ONENET_DRV_MQTT_MSG_SUBACK);
    msg.SetU8Value(mqttCmd.GetReturnCode());
    msg.SetU16Value(mqttCmd.GetPacketIdentifier());
    NotifyObserver(MODULE_ONENET_MANAGER, msg);
    return 0;
}

int32_t OneNetDriver::HandleMqttUnsubscribe(const std::string& bytes)
{
    return 0;
}

int32_t OneNetDriver::HandleMqttUnsubAck(const std::string& bytes)
{
    return 0;
}

int32_t OneNetDriver::HandleMqttPingReq(const std::string& bytes)
{
    return 0;
}

int32_t OneNetDriver::HandleMqttPingResp(const std::string& bytes)
{
    MqttPingresq mqttCmd;
    int32_t ret = mqttCmd.Decode(bytes);
    CHECK_ONENET_RET_VALIDITY(ret);

    SprMsg msg(SIG_ID_ONENET_DRV_MQTT_MSG_PINGRESP);
    ret = SendMsg(msg);
    return ret;
}

int32_t OneNetDriver::HandleMqttDisconnect(const std::string& bytes)
{
    return 0;
}


int32_t OneNetDriver::ProcessMsg(const SprMsg& msg)
{
    SPR_LOGD("Recv msg: %s on <%s:%s>\n", GetSigName(msg.GetMsgId()),
        GetLev1StateString(mCurLev1State), GetLev2StateString(mCurLev2State));

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
