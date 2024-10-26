/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetManager.h
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
#ifndef __ONENET_MANAGER_H__
#define __ONENET_MANAGER_H__

#include <map>
#include <string>
#include <memory>
#include "OneNetDevice.h"
#include "PPipe.h"
#include "SprObserverWithMQueue.h"

#ifdef ENUM_OR_STRING
#undef ENUM_OR_STRING
#endif
#define ENUM_OR_STRING(x) x

// 一级状态
#define ONENET_MGR_LEV1_MACROS                      \
    ENUM_OR_STRING(LEV1_ONENET_MGR_ANY),            \
    ENUM_OR_STRING(LEV1_ONENET_MGR_IDLE),           \
    ENUM_OR_STRING(LEV1_ONENET_MGR_CONNECTING),     \
    ENUM_OR_STRING(LEV1_ONENET_MGR_CONNECTED),      \
    ENUM_OR_STRING(LEV1_ONENET_MGR_DISCONNECTED),   \
    ENUM_OR_STRING(LEV1_ONENET_MGR_BUTT)

enum EOneNetMgrLev1State
{
    ONENET_MGR_LEV1_MACROS
};

// 二级状态
#define ONENET_MGR_LEV2_MACROS                      \
    ENUM_OR_STRING(LEV2_ONENET_MGR_ANY),            \
    ENUM_OR_STRING(LEV2_ONENET_MGR_BUTT)

enum EOneNetMgrLev2State
{
    ONENET_MGR_LEV2_MACROS
};

enum EMqttConnackCode
{
    MQTT_CONNECT_ACCEPTED = 0,
    MQTT_CONNECT_REFUSED_UNACCEPTABLE_PROTOCOL_VERSION,
    MQTT_CONNECT_REFUSED_IDENTIFIER_REJECTED,
    MQTT_CONNECT_REFUSED_SERVER_UNAVAILABLE,
    MQTT_CONNECT_REFUSED_BAD_USERNAME_OR_PASSWORD,
    MQTT_CONNECT_REFUSED_NOT_AUTHORIZED,
    MQTT_CONNECT_BUTT
};

struct OneNetDevInfo
{
    std::string expirationTime;
    std::string oneDevName;
    std::string oneProductID;
    std::string oneKey;
    std::string oneToken;

    void Clear();
};

class OneNetManager : public SprObserverWithMQueue
{
public:
    explicit OneNetManager(ModuleIDType id, const std::string& name);
    ~OneNetManager();

    int32_t ProcessMsg(const SprMsg& msg) override;

private:
    int32_t Init() override;

    /* 初始化调试环境 */
    int32_t InitDebugDetails();

    /* 初始化OneNet设备 */
    int32_t InitOneNetDevices(const std::vector<OneNetDevInfo>& devices);

    /* 加载OneNet设备配置文件 */
    int32_t LoadOneNetDevicesCfgFile(const std::string& cfgPath, std::vector<OneNetDevInfo>& devices);

    /* 更新一级状态 */
    void SetLev1State(EOneNetMgrLev1State state);
    EOneNetMgrLev1State GetLev1State();
    const char* GetLev1StateString(EOneNetMgrLev1State state);

    /* 更新二级状态 */
    void SetLev2State(EOneNetMgrLev2State state);
    EOneNetMgrLev2State GetLev2State();
    const char* GetLev2StateString(EOneNetMgrLev2State state);

    /* 启动OneNet心跳 */
    void StartTimerToPingOneNet(int32_t intervalInMSec);

    /* 启动数据上报定时器 */
    void StartTimerToReportData(int32_t intervalInMSec);

    /* 通知消息到指定OneNetDevice */
    void NotifyMsgToOneNetDevice(const std::string& devModule, const SprMsg& msg);

    /* 调试函数 */
    void DebugUsage(void* args);
    void DebugEnableDumpLog(void* args);
    void DebugActiveDevice(void* args);
    void DebugDeactiveDevice(void* args);

    /* 消息响应函数 */
    void MsgRespondActiveDeviceConnect(const SprMsg& msg);
    void MsgRespondReactiveCurDeviceConnect(const SprMsg& msg);
    void MsgRespondDeactiveDeviceDisconnect(const SprMsg& msg);
    void MsgRespondMqttConnAck(const SprMsg& msg);
    void MsgRespondMqttSubAck(const SprMsg& msg);
    void MsgRespondMqttPingTimerEvent(const SprMsg& msg);
    void MsgRespondMqttReportTimerEvent(const SprMsg& msg);
    void MsgRespondMqttDisconnect(const SprMsg& msg);
    void MsgRespondUnexpectedState(const SprMsg& msg);
    void MsgRespondUnexpectedMsg(const SprMsg& msg);

private:
    bool mEnablePingTimer;
    bool mEnableReportTimer;
    uint32_t mReConnectReqCnt;
    uint32_t mReConnectRspCnt;
    using StateTransitionType =
    InternalDefs::StateTransition<  EOneNetMgrLev1State,
                                    EOneNetMgrLev2State,
                                    InternalDefs::ESprSigId,
                                    OneNetManager,
                                    SprMsg>;
    static std::vector<StateTransitionType> mStateTable;
    EOneNetMgrLev1State mCurLev1State;
    EOneNetMgrLev2State mCurLev2State;
    std::string mCurActiveDevice;
    std::shared_ptr<PPipe> mDebugFileNode;
    std::map<std::string, std::shared_ptr<OneNetDevice>> mOneDeviceMap;

    using DebugCmdFunc = void (OneNetManager::*)(void*);
    std::map<std::string, std::pair<std::string, DebugCmdFunc>> mDebugCmdMap;
};

#endif // __ONENET_MANAGER_H__
