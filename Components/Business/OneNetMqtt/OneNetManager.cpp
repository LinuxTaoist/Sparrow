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
#include <fstream>
#include <sstream>
#include <algorithm>
#include "SprLog.h"
#include "CoreTypeDefs.h"
#include "GeneralUtils.h"
#include "SprDebugNode.h"
#include "CommonErrorCodes.h"
#include "OneNetManager.h"

using namespace std;
using namespace InternalDefs;

#define LOG_TAG "OneNetMgr"
#define DEFAULT_DATA_REPORT_INTERVAL    10   // sec
#define DEFAULT_PING_TIMER_INTERVAL     60    // sec
#define ONENET_DEVICE_NUM_LIMIT         5
#define ONENET_DEVICES_CFG_PATH         "OneNetDevices.conf"

void OneNetDevInfo::Clear()
{
    expirationTime.clear();
    oneDevName.clear();
    oneProductID.clear();
    oneKey.clear();
    oneToken.clear();
}

vector <StateTransition <   EOneNetMgrLev1State,
                            EOneNetMgrLev2State,
                            ESprSigId,
                            OneNetManager,
                            SprMsg> >
OneNetManager::mStateTable =
{
    // =============================================================
    // All States for SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT
    // ============================================================
    { LEV1_ONENET_MGR_IDLE, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT,
      &OneNetManager::MsgRespondActiveDeviceConnect
    },

    { LEV1_ONENET_MGR_DISCONNECTED, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT,
      &OneNetManager::MsgRespondActiveDeviceConnect
    },

    { LEV1_ONENET_MGR_ANY, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT,
      &OneNetManager::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_MGR_REACTIVE_CUR_DEVICE_CONNECT
    // ============================================================
    { LEV1_ONENET_MGR_IDLE, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_REACTIVE_CUR_DEVICE_CONNECT,
      &OneNetManager::MsgRespondReactiveCurDeviceConnect
    },

    { LEV1_ONENET_MGR_DISCONNECTED, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_REACTIVE_CUR_DEVICE_CONNECT,
      &OneNetManager::MsgRespondReactiveCurDeviceConnect
    },

    { LEV1_ONENET_MGR_CONNECTING, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_REACTIVE_CUR_DEVICE_CONNECT,
      &OneNetManager::MsgRespondReactiveCurDeviceConnect
    },

    { LEV1_ONENET_MGR_ANY, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_REACTIVE_CUR_DEVICE_CONNECT,
      &OneNetManager::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_MGR_DEACTIVE_DEVICE
    // ============================================================
    { LEV1_ONENET_MGR_CONNECTED, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_DEACTIVE_DEVICE,
      &OneNetManager::MsgRespondDeactiveDevice
    },

    { LEV1_ONENET_MGR_CONNECTING, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_DEACTIVE_DEVICE,
      &OneNetManager::MsgRespondDeactiveDevice
    },

    { LEV1_ONENET_MGR_ANY, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_DEACTIVE_DEVICE,
      &OneNetManager::MsgRespondUnexpectedState
    },

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
    // All States for SIG_ID_ONENET_DRV_MQTT_MSG_SUBACK
    // =============================================================
    { LEV1_ONENET_MGR_CONNECTED, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_SUBACK,
      &OneNetManager::MsgRespondMqttSubAck
    },

    { LEV1_ONENET_MGR_ANY, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_SUBACK,
      &OneNetManager::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_DRV_MQTT_MSG_PINGRESP
    // =============================================================
    { LEV1_ONENET_MGR_CONNECTED, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_PINGRESP,
      &OneNetManager::MsgRespondMqttPingResp
    },

    { LEV1_ONENET_MGR_ANY, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_PINGRESP,
      &OneNetManager::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_MGR_PING_TIMER_EVENT
    // =============================================================
    { LEV1_ONENET_MGR_CONNECTED, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_PING_TIMER_EVENT,
      &OneNetManager::MsgRespondMqttPingTimerEvent
    },

    { LEV1_ONENET_MGR_CONNECTING, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_PING_TIMER_EVENT,
      &OneNetManager::MsgRespondMqttPingTimerEvent
    },

    { LEV1_ONENET_MGR_DISCONNECTED, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_PING_TIMER_EVENT,
      &OneNetManager::MsgRespondMqttPingTimerEvent
    },

    { LEV1_ONENET_MGR_ANY, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_DRV_MQTT_MSG_CONNACK,
      &OneNetManager::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_MGR_DATA_REPORT_TIMER_EVENT
    // =============================================================
    { LEV1_ONENET_MGR_CONNECTED, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_DATA_REPORT_TIMER_EVENT,
      &OneNetManager::MsgRespondMqttReportTimerEvent
    },

    { LEV1_ONENET_MGR_CONNECTING, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_DATA_REPORT_TIMER_EVENT,
      &OneNetManager::MsgRespondMqttReportTimerEvent
    },

    { LEV1_ONENET_MGR_DISCONNECTED, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_DATA_REPORT_TIMER_EVENT,
      &OneNetManager::MsgRespondMqttReportTimerEvent
    },

    { LEV1_ONENET_MGR_ANY, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_DATA_REPORT_TIMER_EVENT,
      &OneNetManager::MsgRespondUnexpectedState
    },

    // =============================================================
    // All States for SIG_ID_ONENET_MGR_DEIVCE_DISCONNECT_PASSIVE
    // =============================================================
    { LEV1_ONENET_MGR_CONNECTING, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_DEIVCE_DISCONNECT_PASSIVE,
      &OneNetManager::MsgRespondDeviceDisconnectPassive
    },

    { LEV1_ONENET_MGR_CONNECTED, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_DEIVCE_DISCONNECT_PASSIVE,
      &OneNetManager::MsgRespondDeviceDisconnectPassive
    },

    { LEV1_ONENET_MGR_ANY, LEV2_ONENET_MGR_ANY,
      SIG_ID_ONENET_MGR_DEIVCE_DISCONNECT_PASSIVE,
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
    mDebugEnable = false;
    mEnablePingTimer = false;
    mEnableReportTimer = false;
    mIsWatingPingResp = false;
    mReConnectReqCnt = 0;
    mReConnectRspCnt = 0;
    mCurLev1State = LEV1_ONENET_MGR_IDLE;
    mCurLev2State = LEV2_ONENET_MGR_ANY;
}

OneNetManager::~OneNetManager()
{
    UnregisterDebugFuncs();
}

int32_t OneNetManager::Init()
{
    SPR_LOGD("OneNetManager Init\n");
    std::vector<OneNetDevInfo> devices;
    int32_t ret = LoadOneNetDevicesCfgFile(ONENET_DEVICES_CFG_PATH, devices);
    if (ret != 0) {
        SPR_LOGE("Load %s failed\n", ONENET_DEVICES_CFG_PATH);
        return -1;
    }

    RegisterDebugFuncs();
    InitOneNetDevices(devices);
    return 0;
}

int32_t OneNetManager::InitOneNetDevices(const std::vector<OneNetDevInfo>& devices)
{
    // 初始化OneNet设备模块, 支持最大个数参考ONENET_DEVICE_NUM_LIMIT
    for (int32_t i = 0; i < (int32_t)devices.size() && i < ONENET_DEVICE_NUM_LIMIT; i++) {
        ModuleIDType id = ModuleIDType(MODULE_ONENET_DEV01 + i);
        string productID = devices[i].oneProductID;
        string devName = devices[i].oneDevName;
        string moduleName = devName;

        auto pDevice = std::make_shared<OneNetDevice>(id, moduleName);
        pDevice->SetExpirationTime(atoi(devices[i].expirationTime.c_str()));
        pDevice->SetProjectID(devices[i].oneProductID);
        pDevice->SetDevName(devices[i].oneDevName);
        pDevice->SetKey(devices[i].oneKey);
        pDevice->SetToken(devices[i].oneToken);
        pDevice->Initialize();
        mOneDeviceMap[moduleName] = pDevice;
        SPR_LOGD("Init OneNet device[%d]: product/{%s}/device/{%s}\n", i, productID.c_str(), devName.c_str());
    }
    return 0;
}

int32_t OneNetManager::LoadOneNetDevicesCfgFile(const std::string& cfgPath, std::vector<OneNetDevInfo>& devices)
{
    std::ifstream configFile(cfgPath);
    if (!configFile.is_open()) {
        SPR_LOGE("Open %s failed\n", cfgPath.c_str());
        return 0;
    }

    std::string line;
    OneNetDevInfo currentDevice;
    bool parsing = false;

    while (getline(configFile, line)) {
        if (line.find("[Device_") != string::npos) {
            if (parsing) {
                devices.push_back(currentDevice);
                currentDevice = OneNetDevInfo();
            }
            parsing = true;
            continue;
        }

        // SPR_LOGD("line: %s\n", line.c_str());
        if (parsing && !line.empty() && line.front() != '[') {
            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos) {
                std::string key = line.substr(0, equalPos);
                std::string value = line.substr(equalPos + 1);

                // SPR_LOGD("key: %s, value: %s\n", key.c_str(), value.c_str());
                if (key == "ExpirationTime")    currentDevice.expirationTime = value;
                else if (key == "OneDevName")   currentDevice.oneDevName = value;
                else if (key == "OneProductID") currentDevice.oneProductID = value;
                else if (key == "OneKey")       currentDevice.oneKey = value;
                else if (key == "OneToken")     currentDevice.oneToken = value;
            }
        }
    }

    if (parsing) {
        devices.push_back(currentDevice);
    }

    return 0;
}

void OneNetManager::SetLev1State(EOneNetMgrLev1State state)
{
    if (mCurLev1State == state) {
        return;
    }

    SPR_LOGD("Lev1 state changed: %s -> %s\n", GetLev1StateString(mCurLev1State), GetLev1StateString(state));
    mCurLev1State = state;
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

    SPR_LOGD("Lev2 state changed: %s -> %s\n", GetLev2StateString(mCurLev2State), GetLev2StateString(state));
    mCurLev2State = state;
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

void OneNetManager::StartTimerToPingOneNet(int32_t intervalInMSec)
{
    if (mEnablePingTimer) {
        SPR_LOGD("Ping timer is already enabled!\n");
        return;
    }

    SPR_LOGD("Enable ping timer, interval: %dms\n", intervalInMSec);
    mEnablePingTimer = true;
    RegisterTimer(0, intervalInMSec, SIG_ID_ONENET_MGR_PING_TIMER_EVENT, 0);
}

void OneNetManager::StopTimerToPingOneNet()
{
    if (!mEnablePingTimer) {
        SPR_LOGD("Ping timer is already disabled!\n");
        return;
    }

    mEnablePingTimer = false;
    UnregisterTimer(SIG_ID_ONENET_MGR_PING_TIMER_EVENT);
}

void OneNetManager::StartTimerToReportData(int32_t intervalInMSec)
{
    if (mEnableReportTimer) {
        SPR_LOGD("Report timer is already enabled!\n");
        return;
    }

    SPR_LOGD("Enable report timer, interval: %dms\n", intervalInMSec);
    mEnableReportTimer = true;
    RegisterTimer(0, intervalInMSec, SIG_ID_ONENET_MGR_DATA_REPORT_TIMER_EVENT, 0);
}

void OneNetManager::StopTimerToReportData()
{
    if (!mEnableReportTimer) {
        SPR_LOGD("Report timer is already disabled!\n");
        return;
    }

    mEnableReportTimer = false;
    UnregisterTimer(SIG_ID_ONENET_MGR_DATA_REPORT_TIMER_EVENT);
}

void OneNetManager::NotifyMsgToOneNetDevice(const std::string& devModule, const SprMsg& msg)
{
    auto it = mOneDeviceMap.find(devModule);
    if (it == mOneDeviceMap.end()) {
        SPR_LOGW("Invalid device module: %s!\n", devModule.c_str());
        return;
    }

    SprMsg copyMsg(msg);
    it->second->SendMsg(copyMsg);
    SPR_LOGD("Notify module device: %s, msg: %s\n", devModule.c_str(), GetSigName(msg.GetMsgId()));
}

/**
 * @brief Process SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT
 *
 * @param[in] msg
 * @return none
 */
void OneNetManager::MsgRespondActiveDeviceConnect(const SprMsg& msg)
{
    SetLev1State(LEV1_ONENET_MGR_CONNECTING);

    // 激活指定设备连接OneNet，记录当前活动设备
    std::string devModule = msg.GetString();
    mCurActiveDevice = devModule;
    NotifyMsgToOneNetDevice(devModule, msg);
}

/**
 * @brief Process SIG_ID_ONENET_MGR_REACTIVE_CUR_DEVICE_CONNECT
 *
 * @param[in] msg
 * @return none
 */
void OneNetManager::MsgRespondReactiveCurDeviceConnect(const SprMsg& msg)
{
    mReConnectReqCnt++;
    SetLev1State(LEV1_ONENET_MGR_CONNECTING);

    SprMsg conMsg(SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT);
    NotifyMsgToOneNetDevice(mCurActiveDevice, conMsg);
}

/**
 * @brief Process SIG_ID_ONENET_MGR_DEACTIVE_DEVICE
 *
 * @param[in] msg
 * @return none
 */
void OneNetManager::MsgRespondDeactiveDevice(const SprMsg& msg)
{
    bool isReconnect = msg.GetBoolValue();
    EOneNetMgrLev1State state = isReconnect ? LEV1_ONENET_MGR_CONNECTING : LEV1_ONENET_MGR_DISCONNECTED;
    SetLev1State(state);

    SprMsg disMsg(msg);
    NotifyMsgToOneNetDevice(mCurActiveDevice, disMsg);
    SPR_LOGD("Deactive device: %s, reconnect: %d, reason: %s\n", mCurActiveDevice.c_str(), isReconnect, msg.GetString().c_str());

    // 记录下线事件至监控组件
    std::string description = "device offline (" + msg.GetString() + ")";
    SendEventToMonitor(ERR_ONENET_MANAGER_OFFLINE, description);

    mReConnectReqCnt = 0;
    mReConnectRspCnt = 0;
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_CONNACK
 *
 * @param[in] msg
 */
void OneNetManager::MsgRespondMqttConnAck(const SprMsg& msg)
{
    bool isConnected = (msg.GetU8Value() == MQTT_CONNECT_ACCEPTED);
    EOneNetMgrLev1State state = isConnected ? LEV1_ONENET_MGR_CONNECTED : LEV1_ONENET_MGR_DISCONNECTED;
    mReConnectRspCnt++;

    SetLev1State(state);
    SprMsg conMsg(SIG_ID_ONENET_MGR_SET_CONNECT_STATUS);
    conMsg.SetBoolValue(isConnected);
    NotifyMsgToOneNetDevice(mCurActiveDevice, conMsg);

    // 记录上线事件至监控组件
    std::string status = isConnected ? "device online" : "device offline";
    std::string description = status + " (ack:" + std::to_string(msg.GetU8Value()) + ", req: "
        + std::to_string(mReConnectReqCnt) + ", rsp: " + std::to_string(mReConnectRspCnt) + ")";
    SendEventToMonitor(ERR_ONENET_MANAGER_ONLINE, description);

    if (!isConnected) {
        SPR_LOGD("Connect failed, server code: %d\n", msg.GetU8Value());
        return;
    }

    int32_t keepAliveInSec = mOneDeviceMap[mCurActiveDevice]->GetKeepAliveIntervalInSec();
    if (keepAliveInSec <= 0) {
        SPR_LOGW("Invaild keep alive interval: %d, set default: %d", keepAliveInSec, DEFAULT_PING_TIMER_INTERVAL);
        keepAliveInSec = DEFAULT_PING_TIMER_INTERVAL;
    }

    // 注册ping定时器，数据上报定时器
    mIsWatingPingResp = false;
    StartTimerToPingOneNet(keepAliveInSec * 1000);
    StartTimerToReportData(DEFAULT_DATA_REPORT_INTERVAL * 1000);
    SPR_LOGD("OneNet return connect code: %d, start ping timer: %ds, report timer: %ds (%d %d)\n",
        msg.GetU8Value(), keepAliveInSec, DEFAULT_DATA_REPORT_INTERVAL, mReConnectReqCnt, mReConnectRspCnt);

    // 登录成功，清除重连计数
    mReConnectReqCnt = 0;
    mReConnectRspCnt = 0;
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_SUBACK
 *
 * @param[in] msg
 */
void OneNetManager::MsgRespondMqttSubAck(const SprMsg& msg)
{
    NotifyMsgToOneNetDevice(mCurActiveDevice, msg);
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_PINGRESP
 *
 * @param msg
 */
void OneNetManager::MsgRespondMqttPingResp(const SprMsg& msg)
{
    mIsWatingPingResp = false;
    SPR_LOGD("Ping response received\n");
}

/**
 * @brief Process SIG_ID_ONENET_MGR_PING_TIMER_EVENT
 *
 * @param[in] msg
 */
void OneNetManager::MsgRespondMqttPingTimerEvent(const SprMsg& msg)
{
    if (mCurLev1State != LEV1_ONENET_MGR_CONNECTED) {
        SPR_LOGD("Device not connect, stop ping timer\n");
        mIsWatingPingResp = false;
        StopTimerToPingOneNet();
        return;
    }

    if (mIsWatingPingResp) {
        SPR_LOGW("Ping timeout, disconnect device\n");
        SprMsg disMsg(SIG_ID_ONENET_MGR_DEACTIVE_DEVICE);
        disMsg.SetBoolValue(true);  // 重连标记: true 重连，false 不重连
        disMsg.SetString("ping timeout");
        SendMsg(disMsg);
        return;
    }

    mIsWatingPingResp = true;
    NotifyMsgToOneNetDevice(mCurActiveDevice, msg);
}

/**
 * @brief Process SIG_ID_ONENET_MGR_DATA_REPORT_TIMER_EVENT
 *
 * @param[in] msg
 */
void OneNetManager::MsgRespondMqttReportTimerEvent(const SprMsg& msg)
{
    if (mCurLev1State != LEV1_ONENET_MGR_CONNECTED) {
        SPR_LOGD("Device not connect, stop report timer\n");
        StopTimerToReportData();
        return;
    }

    NotifyMsgToOneNetDevice(mCurActiveDevice, msg);
}

/**
 * @brief Process SIG_ID_ONENET_MGR_DEIVCE_DISCONNECT_PASSIVE
 *
 * @param[in] msg
 * @return none
 */
void OneNetManager::MsgRespondDeviceDisconnectPassive(const SprMsg& msg)
{
    SetLev1State(LEV1_ONENET_MGR_CONNECTING);
    SprMsg disMsg(SIG_ID_ONENET_MGR_SET_CONNECT_STATUS);
    disMsg.SetBoolValue(false);
    NotifyMsgToOneNetDevice(mCurActiveDevice, disMsg);

    // 记录下线事件至监控组件
    std::string description = "device offline (" + msg.GetString() + ")";
    SendEventToMonitor(ERR_ONENET_MANAGER_OFFLINE, description);
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

void OneNetManager::RegisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    p->RegisterCmd(mModuleName, "OnOffLog",    "Enable log on-off", std::bind(&OneNetManager::DebugEnableDumpLog,  this, std::placeholders::_1));
    p->RegisterCmd(mModuleName, "DeviceList",  "Show device list",  std::bind(&OneNetManager::DebugDeviceList,     this, std::placeholders::_1));
    p->RegisterCmd(mModuleName, "Active",      "Active device",     std::bind(&OneNetManager::DebugActiveDevice,   this, std::placeholders::_1));
    p->RegisterCmd(mModuleName, "Deactive",    "Deactive device",   std::bind(&OneNetManager::DebugDeactiveDevice, this, std::placeholders::_1));
}

void OneNetManager::UnregisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    SPR_LOGD("Unregister %s all debug funcs\n", mModuleName.c_str());
    p->UnregisterCmd(mModuleName);
}

void OneNetManager::DebugEnableDumpLog(const std::vector<std::string>& args)
{
    mDebugEnable = !mDebugEnable;
    SprMsg msg(SIG_ID_ONENET_MGR_DEBUG_ENABLE);
    msg.SetBoolValue(mDebugEnable);
    NotifyAllObserver(msg);
    SPR_LOGD("mDebugEnable = %d\n", mDebugEnable);
}

void OneNetManager::DebugDeviceList(const std::vector<std::string>& args)
{
    SPR_LOGD("Device List:\n");
    int32_t i = 0;
    for (auto it = mOneDeviceMap.begin(); it != mOneDeviceMap.end(); it++) {
        SPR_LOGD(" %d. %s\n", ++i, it->first.c_str());
    }
}

void OneNetManager::DebugActiveDevice(const std::vector<std::string>& args)
{
    if (args.size() != 2) {
        SPR_LOGE("Invalid args size: %d\n", args.size());
        SPR_LOGE("Usage: echo active {device_name} > /tmp/debug_onenet\n");
        return;
    }

    SPR_LOGD("Debug Active Device [%s]\n", args[1].c_str());
    SprMsg msg(SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT);
    msg.SetString(args[1]);
    SendMsg(msg);
}

void OneNetManager::DebugDeactiveDevice(const std::vector<std::string>& args)
{
    SPR_LOGD("Debug Deactive Device\n");
    SprMsg disMsg(SIG_ID_ONENET_MGR_DEACTIVE_DEVICE);
    disMsg.SetString("deactive by node debug");
    SendMsg(disMsg);
}
