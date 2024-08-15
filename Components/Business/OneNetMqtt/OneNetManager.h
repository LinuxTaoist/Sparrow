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

#include <string>
#include "SprObserverWithMQueue.h"

// 一级状态:
enum EOneNetMgrLev1State
{
    LEV1_ONENET_MGR_ANY         = 0x00,
    LEV1_ONENET_MGR_IDLE,
    LEV1_ONENET_MGR_CONNECTING,
    LEV1_ONENET_MGR_CONNECTED,
    LEV1_ONENET_MGR_DISCONNECTED,
    LEV1_ONENET_MGR_BUTT
};

//二级状态:
enum EOneNetMgrLev2State
{
    LEV2_ONENET_MGR_ANY         = 0x00,
    LEV2_ONENET_MGR_BUTT
};

class OneNetManager : public SprObserverWithMQueue
{
public:
    ~OneNetManager();
    static OneNetManager* GetInstance(ModuleIDType id, const std::string& name);

    int32_t Init() override;
    int32_t ProcessMsg(const SprMsg& msg) override;

private:
    explicit OneNetManager(ModuleIDType id, const std::string& name);

    /* 更新一级状态 */
    void SetLev1State(EOneNetMgrLev1State state);
    EOneNetMgrLev1State GetLev1State();

    /* 更新二级状态 */
    void SetLev2State(EOneNetMgrLev2State state);
    EOneNetMgrLev2State GetLev2State();

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
};


#endif // __ONENET_MANAGER_H__
