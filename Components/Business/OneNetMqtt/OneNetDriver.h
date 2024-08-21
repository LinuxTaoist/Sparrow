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

#include <string>
#include "MqttProtocol.h"
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

    /**
     * @brief Init business
     *
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t Init() override;

    /**
     * @brief Init Unix PIPE to send and cache socket bytes
     *
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t InitUnixPIPE();

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
    virtual int32_t DumpSocketBytes(const std::string& tag, const std::string& bytes);
    virtual int32_t DumpSocketBytesWithAscall(const std::string& bytes);

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
    void MsgRespondMqttMsgConnect(const SprMsg& msg);
    void MsgRespondMqttMsgConnack(const SprMsg& msg);
    void MsgRespondUnexpectedState(const SprMsg& msg);
    void MsgRespondUnexpectedMsg(const SprMsg& msg);

    /**
     * @brief Send mqtt cmd bytes
     */
    int32_t SendMqttConnect(const std::string& payload);        // 3.1 CONNECT
    int32_t SendMqttConnack();                                  // 3.2 CONNACK
    int32_t SendMqttPublish(uint16_t cmd);                      // 3.3 PUBLISH
    int32_t SendMqttPubAck(uint16_t cmd);                       // 3.4 PUBACK
    int32_t SendMqttPubRec(uint16_t cmd);                       // 3.5 PUBREC
    int32_t SendMqttPubRel(uint16_t cmd);                       // 3.6 PUBREL
    int32_t SendMqttPubComp(uint16_t cmd);                      // 3.7 PUBCOMP
    int32_t SendMqttSubscribe(uint16_t cmd);                    // 3.8 SUBSCRIBE
    int32_t SendMqttSubAck(uint16_t cmd);                       // 3.9 SUBACK
    int32_t SendMqttUnsubscribe(uint16_t cmd);                  // 3.10 UNSUBSCRIBE
    int32_t SendMqttUnsubAck(uint16_t cmd);                     // 3.11 UNSUBACK
    int32_t SendMqttPingReq();                                  // 3.12 PINGREQ
    int32_t SendMqttPingResp();                                 // 3.13 PINGRESP
    int32_t SendMqttDisconnect();                               // 3.14 DISCONNECT

    int32_t DispatchMqttBytes(const std::string& bytes);
    int32_t HandleMqttConnect(const std::string& bytes);        // 3.1 CONNECT
    int32_t HandleMqttConnack(const std::string& bytes);        // 3.2 CONNACK
    int32_t HandleMqttPublish(const std::string& bytes);        // 3.3 PUBLISH
    int32_t HandleMqttPubAck(const std::string& bytes);         // 3.4 PUBACK
    int32_t HandleMqttPubRec(const std::string& bytes);         // 3.5 PUBREC
    int32_t HandleMqttPubRel(const std::string& bytes);         // 3.6 PUBREL
    int32_t HandleMqttPubComp(const std::string& bytes);        // 3.7 PUBCOMP
    int32_t HandleMqttSubscribe(const std::string& bytes);      // 3.8 SUBSCRIBE
    int32_t HandleMqttSubAck(const std::string& bytes);         // 3.9 SUBACK
    int32_t HandleMqttUnsubscribe(const std::string& bytes);    // 3.10 UNSUBSCRIBE
    int32_t HandleMqttUnsubAck(const std::string& bytes);       // 3.11 UNSUBACK
    int32_t HandleMqttPingReq(const std::string& bytes);        // 3.12 PINGREQ
    int32_t HandleMqttPingResp(const std::string& bytes);       // 3.13 PINGRESP
    int32_t HandleMqttDisconnect(const std::string& bytes);     // 3.14 DISCONNECT

    /**
     * @brief Send mqtt bytes to remote server
     *
     * @param bytes
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t SendMqttBytes(const std::string& bytes);

private:
    bool mEnableReconTimer;
    std::string mSockBuffer;
    std::string mOneNetHost;
    uint16_t    mOneNetPort;
    EOneNetDrvLev1State mCurLev1State;
    EOneNetDrvLev2State mCurLev2State;
    SprObserverWithSocket* mOneSocketPtr;
    SprObserverWithSocket* mSendPIPEPtr; // unix pipe for send mqtt bytes
    SprObserverWithSocket* mRecvPIPEPtr; // unix pipe for recv mqtt bytes

    using StateTransitionType = InternalDefs::StateTransition<EOneNetDrvLev1State,
                                                EOneNetDrvLev2State,
                                                InternalDefs::ESprSigId,
                                                OneNetDriver,
                                                SprMsg>;
    static std::vector<StateTransitionType> mStateTable;
};

#endif // __ONENET_DRIVER_H__
