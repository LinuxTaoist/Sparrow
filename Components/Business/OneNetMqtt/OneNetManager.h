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
#include "SprObserverWithMQueue.h"

#ifdef ENUM_OR_STRING
#undef ENUM_OR_STRING
#endif
#define ENUM_OR_STRING(x) x

// 一级状态:
#define ONENET_MGR_LEV1_MACROS                      \
    ENUM_OR_STRING(LEV1_ONENET_MGR_ANY),            \
    ENUM_OR_STRING(LEV1_ONENET_MGR_IDLE),           \
    ENUM_OR_STRING(LEV1_ONENET_MGR_CONNECTING),     \
    ENUM_OR_STRING(LEV1_ONENET_MGR_CONNECTED),      \
    ENUM_OR_STRING(LEV1_ONENET_MGR_DISCONNECTED),   \
    ENUM_OR_STRING(LEV1_ONENET_MGR_BUTT),

enum EOneNetMgrLev1State
{
    ONENET_MGR_LEV1_MACROS
};

//二级状态:
#define ONENET_MGR_LEV2_MACROS                      \
    ENUM_OR_STRING(LEV2_ONENET_MGR_ANY),            \
    ENUM_OR_STRING(LEV2_ONENET_MGR_BUTT)

enum EOneNetMgrLev2State
{
    ONENET_MGR_LEV2_MACROS
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
    ~OneNetManager();
    static OneNetManager* GetInstance(ModuleIDType id, const std::string& name);

    int32_t ProcessMsg(const SprMsg& msg) override;

private:
    explicit OneNetManager(ModuleIDType id, const std::string& name);
    int32_t Init() override;

    /* 初始化OneNet设备 */
    int32_t InitOneNetDevices(const OneNetDevInfo& devInfo);

    /* 加载OneNet设备配置文件 */
    int32_t LoadOneNetDevicesCfgFile(const std::string& cfgPath, std::vector<OneNetDevInfo> &devices);

    /* 更新一级状态 */
    void SetLev1State(EOneNetMgrLev1State state);
    EOneNetMgrLev1State GetLev1State();
    const char* GetLev1StateString(EOneNetMgrLev1State state);

    /* 更新二级状态 */
    void SetLev2State(EOneNetMgrLev2State state);
    EOneNetMgrLev2State GetLev2State();
    const char* GetLev2StateString(EOneNetMgrLev2State state);

    /* 消息响应函数 */
    void MsgRespondMqttConnect(const SprMsg& msg);
    void MsgRespondMqttConnAck(const SprMsg& msg);
    void MsgRespondMqttDisconnect(const SprMsg& msg);
    void MsgRespondUnexpectedState(const SprMsg& msg);
    void MsgRespondUnexpectedMsg(const SprMsg& msg);

private:
    using StateTransitionType =
    InternalDefs::StateTransition<  EOneNetMgrLev1State,
                                    EOneNetMgrLev2State,
                                    InternalDefs::ESprSigId,
                                    OneNetManager,
                                    SprMsg>;
    static std::vector<StateTransitionType> mStateTable;
    EOneNetMgrLev1State mCurLev1State;
    EOneNetMgrLev2State mCurLev2State;
    std::map<std::string, std::shared_ptr<OneNetDevice>> mOneDeviceMap;
};

#endif // __ONENET_MANAGER_H__
