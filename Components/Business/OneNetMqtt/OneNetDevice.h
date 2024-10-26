/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetDevice.h
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
#ifndef __ONENET_DEVICE_H__
#define __ONENET_DEVICE_H__

#include <set>
#include <string>
#include "SprObserverWithMQueue.h"

enum ETopicStatus
{
    TOPIC_STATUS_IDLE = 0,
    TOPIC_STATUS_WAIT_ACK,
    TOPIC_STATUS_SUBSCRIBED_SUCCESS,
    TOPIC_STATUS_SUBSCRIBING_FAIL,
    TOPIC_STATUS_UNSUBSCRIBED,
    TOPIC_STATUS_BUTT
};

struct OneNetTopic
{
    ETopicStatus status;
    uint16_t identifier;
    std::string topic;
};

struct BatteryStatus
{
    std::string identifier;
    std::string perIdentifier;
    float percent;
    std::string vltIdentifier;
    int32_t voltage;
};

struct SystemInfo
{
    std::string identifier;
    std::string verIdentifier;
    std::string version;
    std::string descIdentifier;
    std::string description;
};

class OneNetDevice : public SprObserverWithMQueue
{
public:
    /**
     * @brief Construct/Destruct
     */
    explicit OneNetDevice(ModuleIDType id, const std::string& name);
    virtual ~OneNetDevice();

    /**
     * @brief Initialize business and check device information
     *
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t Init() override;

    /**
     * @brief  Process message from message queue received
     *
     * @param msg
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t ProcessMsg(const SprMsg& msg) override;

    /**
     * @brief Dump device information for debug
     *
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t DumpDeviceInformation();

    /**
     * @brief Set/Get device information for OneNet
     * @return device information
     */
    void SetExpirationTime(int32_t expirationTime) { mExpirationTime = expirationTime; }
    void SetDevName(const std::string& devName) { mOneDevName = devName; }
    void SetProjectID(const std::string& projectID) { mOneProductID = projectID; }
    void SetKey(const std::string& key) { mOneKey = key; }
    void SetToken(const std::string& token) { mOneToken = token; }
    int32_t GetExpirationTime() { return mExpirationTime; }
    std::string GetDevName() { return mOneDevName; }
    std::string GetProjectID() { return mOneProductID; }
    std::string GetKey() { return mOneKey; }
    std::string GetToken() { return mOneToken; }

    int32_t InitTopicList();
    bool GetConnectStatus() { return mConnectStatus; }
    int32_t GetKeepAliveIntervalInSec() { return mKeepAliveIntervalInSec; }


private:
    /**
     * @brief Verify device information
     *
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t VerifyDeviceDetails();

    uint16_t GetUnusedIdentity();
    uint16_t ReleaseIdentity(uint16_t identity);
    void ResetAllTopics();
    void StartSubscribeTopic();

    /**
     * @brief Get device attribute
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t GetCPUUsage(float& cpuUsage);
    int32_t GetDiskUsage(int32_t& diskUsage);
    int32_t GetMemoryUsage(int32_t& memoryUsage);
    int32_t GetModelName(std::string& model);
    int32_t GetLaunchTime(int32_t& launchTime);
    int32_t GetBatteryStatus(BatteryStatus& batteryStatus);
    int32_t GetSystemInfo(SystemInfo& systemInfo);
    int32_t AddCPUUsageJsonNode(void* pNode);
    int32_t AddDiskUsageJsonNode(void* pNode);
    int32_t AddMemoryUsageJsonNode(void* pNode);
    int32_t AddModelNameJsonNode(void* pNode);
    int32_t AddLaunchTimeJsonNode(void* pNode);
    int32_t AddBatteryStatusJsonNode(void* pNode);
    int32_t AddSystemInfoJsonNode(void* pNode);
    std::string PreparePublishPayloadJson();

    /**
     * @brief message handle function
     *
     * @param msg
     */
    void MsgRespondActiveDeviceConnect(const SprMsg& msg);
    void MsgRespondDeactiveDeviceDisconnect(const SprMsg& msg);
    void MsgRespondSetConnectStatus(const SprMsg& msg);
    void MsgRespondSubscribeTopic(const SprMsg& msg);
    void MsgRespondSubscribeTopicAck(const SprMsg& msg);
    void MsgRespondPingTimerEvent(const SprMsg& msg);
    void MsgRespondDataReportTimerEvent(const SprMsg& msg);

private:
    int32_t mExpirationTime;    // Token过期时间
    std::string mOneDevName;    // 设备名称/ID
    std::string mOneProductID;  // 产品ID
    std::string mOneKey;        // 设备密钥
    std::string mOneToken;      // 设备Token

    bool mConnectStatus;                    // 设备连接状态
    int32_t mKeepAliveIntervalInSec;        // 设备保活时间间隔
    std::string mCurTopic;                  // 当前正在订阅的主题
    std::set<uint16_t> mUsedIdentities;     // 已使用的identity列表
    std::map<std::string, OneNetTopic> mAllTopicMap;    // 订阅主题列表, key：topicID
};

#endif // __ONENET_DEVICE_H__
