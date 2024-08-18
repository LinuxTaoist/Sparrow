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
#include "SprObserverWithSocket.h"
#include "SprObserverWithMQueue.h"


#define ONENET_DRV_LEV1_MACROS                      \
    ENUM_OR_STRING(LEV1_SOCKET_ANY),                \
    ENUM_OR_STRING(LEV1_SOCKET_IDLE),               \
    ENUM_OR_STRING(LEV1_SOCKET_CONNECTING),         \
    ENUM_OR_STRING(LEV1_SOCKET_CONNECTED),          \
    ENUM_OR_STRING(LEV1_SOCKET_DISCONNECTED),       \
    ENUM_OR_STRING(LEV1_SOCKET_BUTT)

// 一级状态:
enum EOneNetDrvLev1State
{
    ONENET_DRV_LEV1_MACROS
};
#define ONENET_DRV_LEV2_MACROS                      \
    ENUM_OR_STRING(LEV2_ONENET_ANY),                \
    ENUM_OR_STRING(LEV2_ONENET_IDLE),               \
    ENUM_OR_STRING(LEV2_ONENET_CONNECTING),         \
    ENUM_OR_STRING(LEV2_ONENET_CONNECTED),          \
    ENUM_OR_STRING(LEV2_ONENET_DISCONNECTED),       \
    ENUM_OR_STRING(LEV2_ONENET_BUTT)

//二级状态:
enum EOneNetDrvLev2State
{
    ONENET_DRV_LEV2_MACROS
};

class OneNetDriver : public SprObserverWithMQueue
{
public:
    ~OneNetDriver();
    static OneNetDriver* GetInstance(ModuleIDType id, const std::string& name);

    int32_t ProcessMsg(const SprMsg& msg) override;

private:
    explicit OneNetDriver(ModuleIDType id, const std::string& name);

    // static void SocketEventCb(int, void*);

    /**
     * @brief Init business
     *
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t Init() override;

    /**
     * @brief Set/Get the level 1 state
     *
     * @param state level 1 state
     * @return level 1 state
     */
    void SetLev1State(EOneNetDrvLev1State state);
    const char* GetLev1StateString(EOneNetDrvLev1State state);
    EOneNetDrvLev1State GetLev1State();

    /**
     * @brief Set/Get the level 2 state
     *
     * @param state level 2 state
     * @return level 2 state
     */
    void SetLev2State(EOneNetDrvLev2State state);
    const char* GetLev2StateString(EOneNetDrvLev2State state);
    EOneNetDrvLev2State GetLev2State();

    /**
     * @brief Dump socket bytes for debug
     *
     * @param bytes socket bytes
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int32_t DumpSocketBytes(const std::string& bytes);

    /**
     * @brief message handle function
     *
     * @param msg
     */
    void MsgRespondSocketConnect(const SprMsg& msg);
    void MsgRespondSocketConnectSuccess(const SprMsg& msg);
    void MsgRespondSocketConnectFail(const SprMsg& msg);
    void MsgRespondSocketReconnect(const SprMsg& msg);
    void MsgRespondSocketReconnectTimerEvent(const SprMsg& msg);
    void MsgRespondSocketDisconnectActive(const SprMsg& msg);
    void MsgRespondSocketDisconnectPassive(const SprMsg& msg);
    void MsgRespondUnexpectedState(const SprMsg& msg);
    void MsgRespondUnexpectedMsg(const SprMsg& msg);

private:
    bool mEnableReconTimer;
    std::mutex  mSockMutex;
    std::string mSockBuffer;
    std::string mOneNetHost;
    uint16_t    mOneNetPort;
    EOneNetDrvLev1State mCurLev1State;
    EOneNetDrvLev2State mCurLev2State;
    SprObserverWithSocket* mOneSocketPtr;

    using StateTransitionType = InternalDefs::StateTransition<EOneNetDrvLev1State,
                                                EOneNetDrvLev2State,
                                                InternalDefs::ESprSigId,
                                                OneNetDriver,
                                                SprMsg>;
    static std::vector<StateTransitionType> mStateTable;
};

#endif // __ONENET_DRIVER_H__
