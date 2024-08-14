/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetDriver.h
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
#ifndef __ONENET_DRIVER_H__
#define __ONENET_DRIVER_H__

#include <mutex>
#include <string>
#include <memory>
#include "PSocket.h"
#include "SprObserver.h"

// 一级状态:
enum EOneNetDrvLev1State
{
    LEV1_SOCKET_ANY         = 0x00,
    LEV1_SOCKET_IDLE,
    LEV1_SOCKET_CONNECTING,
    LEV1_SOCKET_CONNECTED,
    LEV1_SOCKET_DISCONNECTED,
    LEV1_SOCKET_BUTT
};

//二级状态:
enum EOneNetDrvLev2State
{
    LEV2_ONENET_ANY         = 0x00,
    LEV2_ONENET_IDLE,
    LEV2_ONENET_CONNECTING,
    LEV2_ONENET_CONNECTED,
    LEV2_ONENET_DISCONNECTED,
    LEV2_ONENET_BUTT
};

class OneNetDriver : public SprObserver
{
public:
    ~OneNetDriver();
    static OneNetDriver* GetInstance(ModuleIDType id, const std::string& name);

    void Init();
    int ProcessMsg(const SprMsg& msg) override;

private:
    explicit OneNetDriver(ModuleIDType id, const std::string& name);

    /* 更新一级状态 */
    void SetLev1State(EOneNetDrvLev1State state);
    EOneNetDrvLev1State GetLev1State();

    /* 更新二级状态 */
    void SetLev2State(EOneNetDrvLev2State state);
    EOneNetDrvLev2State GetLev2State();

    /* 消息响应函数 */
    void MsgRespondSocketConnect(const SprMsg& msg);
    void MsgRespondSocketDisconnectActive(const SprMsg& msg);
    void MsgRespondSocketDisconnectPassive(const SprMsg& msg);
    void MsgRespondUnexpectedState(const SprMsg& msg);
    void MsgRespondUnexpectedMsg(const SprMsg& msg);

private:
    std::mutex  mSockMutex;
    std::string mSockBuffer;
    std::string mOneNetHost;
    uint16_t    mOneNetPort;
    EOneNetDrvLev1State mCurLev1State;
    EOneNetDrvLev2State mCurLev2State;
    std::shared_ptr<PSocket> mSocketPtr;

    using StateTransitionType = InternalDefs::StateTransition<EOneNetDrvLev1State,
                                                EOneNetDrvLev2State,
                                                InternalDefs::ESprSigId,
                                                OneNetDriver,
                                                SprMsg>;
    static std::vector<StateTransitionType> mStateTable;
};

#endif // __ONENET_DRIVER_H__
