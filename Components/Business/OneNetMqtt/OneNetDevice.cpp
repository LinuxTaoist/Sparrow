/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetDevice.cpp
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
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include "SprLog.h"
#include "cJSON.h"
#include "OneNetCommon.h"
#include "OneNetDevice.h"

using namespace InternalDefs;

#define SPR_LOGI(fmt, args...) LOGI("OneNetDev", "[%s] " fmt, mOneDevName.c_str(), ##args)
#define SPR_LOGD(fmt, args...) LOGD("OneNetDev", "[%s] " fmt, mOneDevName.c_str(), ##args)
#define SPR_LOGW(fmt, args...) LOGW("OneNetDev", "[%s] " fmt, mOneDevName.c_str(), ##args)
#define SPR_LOGE(fmt, args...) LOGE("OneNetDev", "[%s] " fmt, mOneDevName.c_str(), ##args)

#define ONENET_DEVICE_CFG_PATH          "OneNetDevices.conf"
#define DEFAULT_KEEP_ALIVE_INTERVAL     60  // 60s

#define TEMPLATE_TOPIC_THING            "$sys/%s/%s/thing/%s"
#define TEMPLATE_TOPIC_CMD              "$sys/%s/%s/cmd/%s"
#define TEMPLATE_TOPIC_THING_POST       "$sys/%s/%s/thing/property/post"

OneNetDevice::OneNetDevice(ModuleIDType id, const std::string& name)
    : SprObserverWithMQueue(id, name), mExpirationTime(0), mConnectStatus(false)
{
    mKeepAliveIntervalInSec = DEFAULT_KEEP_ALIVE_INTERVAL;
}

OneNetDevice::~OneNetDevice()
{
}

int32_t OneNetDevice::Init()
{
    DumpDeviceInformation();
    VerifyDeviceDetails();
    InitTopicList();
    return 0;
}

int32_t OneNetDevice::DumpDeviceInformation()
{
    SPR_LOGI("------------------ Dump OneNet Device Information ------------------\n");
    SPR_LOGI("- OneExpTime      : %d\n", mExpirationTime);
    SPR_LOGI("- OneDevName      : %s\n", mOneDevName.c_str());
    SPR_LOGI("- OneProductID    : %s\n", mOneProductID.c_str());
    SPR_LOGI("- OneKey          : %s\n", mOneKey.c_str());
    SPR_LOGI("- OneToken        : %s\n", mOneToken.c_str());
    SPR_LOGI("--------------------------------------------------------------------\n");
    return 0;
}

int32_t OneNetDevice::VerifyDeviceDetails()
{
    if (mOneDevName.empty()) {
        SPR_LOGE("Device name is empty!\n");
        return -1;
    }
    if (mOneProductID.empty()) {
        SPR_LOGE("Project ID is empty!\n");
        return -1;
    }
    if (mOneKey.empty()) {
        SPR_LOGE("API key is empty!\n");
        return -1;
    }
    if (mOneToken.empty()) {
        SPR_LOGE("API token is empty!\n");
        return -1;
    }
    if (mExpirationTime <= 0) {
        SPR_LOGE("Expiration time is invalid!\n");
        return -1;
    }

    return 0;
}

int32_t OneNetDevice::InitTopicList()
{
    char topicThingAll[128] = {0};
    char topicCmd[128] = {0};

    // $sys/{pid}/{device-name}/thing/property/#
    // $sys/{pid}/{device-name}/cmd/#
    snprintf(topicThingAll, sizeof(topicThingAll), TEMPLATE_TOPIC_THING, mOneProductID.c_str(), mOneDevName.c_str(), "property/post/reply");
    snprintf(topicCmd, sizeof(topicCmd), TEMPLATE_TOPIC_CMD, mOneProductID.c_str(), mOneDevName.c_str(), "#");
    mAllTopicMap[topicThingAll] = {TOPIC_STATUS_IDLE, 0, topicThingAll};
    mAllTopicMap[topicCmd] = {TOPIC_STATUS_IDLE, 0, topicCmd};

    for (const auto &topic : mAllTopicMap) {
        SPR_LOGI("OneNet Topic: [%s]\n", topic.first.c_str());
    }

    return 0;
}

uint16_t OneNetDevice::GetUnusedIdentity()
{
    uint16_t identity = 1;
    if (mUsedIdentities.empty()) {
        mUsedIdentities.insert(identity);
        return identity;
    }

    if (*(--mUsedIdentities.end()) == (uint16_t)mUsedIdentities.size()) {
        identity = (uint16_t)(mUsedIdentities.size() + 1);
        mUsedIdentities.insert(identity);
        return identity;
    }

    for (uint16_t i = 1; i < (uint16_t)mUsedIdentities.size(); i++) {
        if (mUsedIdentities.find(i) == mUsedIdentities.end()) {
            identity = i;
            mUsedIdentities.insert(identity);
            return identity;
        }
    }

    return identity;
}

uint16_t OneNetDevice::ReleaseIdentity(uint16_t identity)
{
    if (mUsedIdentities.find(identity) == mUsedIdentities.end()) {
        SPR_LOGW("Identity %d is not used!\n", identity);
        return 0;
    }

    mUsedIdentities.erase(identity);
    return identity;
}

void OneNetDevice::ResetAllTopics()
{
    for (auto &topic : mAllTopicMap) {
        topic.second.status = TOPIC_STATUS_IDLE;
    }
}

void OneNetDevice::StartSubscribeTopic()
{
    SprMsg msg(SIG_ID_ONENET_DEV_SUBSCRIBE_TOPIC);
    SendMsg(msg);
}

int32_t OneNetDevice::AddCPUUsageJsonNode(void* pNode)
{
    cJSON* pParamNode = (cJSON*)pNode;
    if (pParamNode == nullptr) {
        SPR_LOGE("pParamNode is nullptr!\n");
        return -1;
    }

    float cpuUsage = 0.0;
    GetCPUUsage(cpuUsage);
    cJSON* pCpuNode = cJSON_CreateObject();
    cJSON_AddNumberToObject(pCpuNode, "value", cpuUsage);
    cJSON_AddItemToObject(pParamNode, "CPU_Usage", pCpuNode);
    return 0;
}

int32_t OneNetDevice::AddDiskUsageJsonNode(void* pNode)
{
    cJSON* pParamNode = (cJSON*)pNode;
    if (pParamNode == nullptr) {
        SPR_LOGE("pParamNode is nullptr!\n");
        return -1;
    }

    int32_t diskUsage = 0;
    GetDiskUsage(diskUsage);
    cJSON* pDiskNode = cJSON_CreateObject();
    cJSON_AddNumberToObject(pDiskNode, "value", diskUsage);
    cJSON_AddItemToObject(pParamNode, "Disk_Usage", pDiskNode);
    return 0;
}

int32_t OneNetDevice::AddMemoryUsageJsonNode(void* pNode)
{
    cJSON* pParamNode = (cJSON*)pNode;
    if (pParamNode == nullptr) {
        SPR_LOGE("pParamNode is nullptr!\n");
        return -1;
    }

    int32_t memoryUsage = 0;
    GetMemoryUsage(memoryUsage);
    cJSON* pMemoryNode = cJSON_CreateObject();
    cJSON_AddNumberToObject(pMemoryNode, "value", memoryUsage);
    cJSON_AddItemToObject(pParamNode, "Memory_Usage", pMemoryNode);
    return 0;
}

int32_t OneNetDevice::AddModelNameJsonNode(void* pNode)
{
    cJSON* pParamNode = (cJSON*)pNode;
    if (pParamNode == nullptr) {
        SPR_LOGE("pParamNode is nullptr!\n");
        return -1;
    }

    std::string modelName;
    GetModelName(modelName);
    cJSON* pModelNode = cJSON_CreateObject();
    cJSON_AddStringToObject(pModelNode, "value", modelName.c_str());
    cJSON_AddItemToObject(pParamNode, "Model", pModelNode);
    return 0;
}

int32_t OneNetDevice::AddLaunchTimeJsonNode(void* pNode)
{
    cJSON* pParamNode = (cJSON*)pNode;
    if (pParamNode == nullptr) {
        SPR_LOGE("pParamNode is nullptr!\n");
        return -1;
    }

    int32_t launchtime = 0;
    GetLaunchTime(launchtime);
    cJSON* pLaunchNode = cJSON_CreateObject();
    cJSON_AddNumberToObject(pLaunchNode, "value", launchtime);
    cJSON_AddItemToObject(pParamNode, "Launch_Time", pLaunchNode);
    return 0;
}

int32_t OneNetDevice::AddBatteryStatusJsonNode(void* pNode)
{
    cJSON* pParamNode = (cJSON*)pNode;
    if (pParamNode == nullptr) {
        SPR_LOGE("pParamNode is nullptr!\n");
        return -1;
    }

    BatteryStatus batteryStatus = {"Battery_Status", "Percent", 0.0, "Voltage", 0};
    GetBatteryStatus(batteryStatus);
    cJSON* pBatNode = cJSON_CreateObject();
    cJSON* pValueNode = cJSON_CreateObject();
    cJSON_AddNumberToObject(pValueNode, batteryStatus.perIdentifier.c_str(), batteryStatus.percent);
    cJSON_AddNumberToObject(pValueNode, batteryStatus.vltIdentifier.c_str(), batteryStatus.voltage);
    cJSON_AddItemToObject(pBatNode, "value", pValueNode);
    cJSON_AddItemToObject(pParamNode, batteryStatus.identifier.c_str(), pBatNode);
    return 0;
}

int32_t OneNetDevice::AddSystemInfoJsonNode(void* pNode)
{
    cJSON* pParamNode = (cJSON*)pNode;
    if (pParamNode == nullptr) {
        SPR_LOGE("pParamNode is nullptr!\n");
        return -1;
    }

    SystemInfo sysInfo = {"system_infomation", "version", "Unkown", "Description", "Unkown"};
    GetSystemInfo(sysInfo);
    cJSON* pSysNode = cJSON_CreateObject();
    cJSON* pValueNode = cJSON_CreateObject();
    cJSON_AddStringToObject(pValueNode, sysInfo.descIdentifier.c_str(), sysInfo.description.c_str());
    cJSON_AddStringToObject(pValueNode, sysInfo.verIdentifier.c_str(), sysInfo.version.c_str());
    cJSON_AddItemToObject(pSysNode, "value", pValueNode);
    cJSON_AddItemToObject(pParamNode, sysInfo.identifier.c_str(), pSysNode);
    return 0;
}
std::string OneNetDevice::PreparePublishPayloadJson()
{
    // payload 格式
    static int32_t id = 0;
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "id", std::to_string(++id).c_str());
    cJSON_AddStringToObject(root, "version", "1.0");
    cJSON* paramsNode = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "params", paramsNode);

    AddBatteryStatusJsonNode(paramsNode);   // 添加电池状态
    AddCPUUsageJsonNode(paramsNode);        // 添加 CPU 使用率
    AddDiskUsageJsonNode(paramsNode);       // 添加磁盘使用率
    AddMemoryUsageJsonNode(paramsNode);     // 添加内存使用率
    AddModelNameJsonNode(paramsNode);       // 添加设备型号
    AddLaunchTimeJsonNode(paramsNode);      // 添加启动时间
    AddSystemInfoJsonNode(paramsNode);      // 添加系统信息

    // 将 cJSON 对象转换为字符串
    std::string payload;
    char* jsonString = cJSON_PrintUnformatted(root);
    if (jsonString != nullptr) {
        payload = jsonString;
        free(jsonString); // 释放分配的内存
        cJSON_Delete(root); // 删除 cJSON 对象
        return payload;
    }

    cJSON_Delete(root); // 即使转换失败也要删除 cJSON 对象
    return payload;
}

int32_t OneNetDevice::GetCPUUsage(float& cpuUsage)
{
    // 读取 /proc/stat 文件以获取 CPU 使用情况
    std::ifstream statFile("/proc/stat");
    if (!statFile.is_open()) {
        return -1;
    }

    std::string line;
    std::getline(statFile, line); // 读取第一行，包含 CPU 信息

    std::istringstream iss(line);
    std::string token;
    iss >> token; // Skip "cpu"

    std::vector<long> values;

    while (iss >> token) {
        values.push_back(std::stol(token, nullptr, 10));
    }

    if (values.size() < 5) {
        return -1;
    }

    long total = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        total += values[i];
    }

    long idle = values[3];
    long nonIdle = total - idle;

    // 计算 CPU 使用率
    if (total > 0) {
        cpuUsage = (nonIdle * 1.0f) / total * 100.0f;
    } else {
        cpuUsage = 0.00f;
    }

    SPR_LOGD("idle: %ld, nonIdle: %ld, total: %ld, cpuUsage: %f%\n", idle, nonIdle, total, cpuUsage);
    return 0;
}

int32_t OneNetDevice::GetDiskUsage(int32_t& diskUsage)
{
    struct statvfs fsinfo;
    if (statvfs("/", &fsinfo) == -1) {
        SPR_LOGE("statvfs failed!\n");
        diskUsage = 0;
        return -1;
    }

    // Calculate total capacity in MB
    diskUsage = static_cast<int32_t>((fsinfo.f_blocks * fsinfo.f_bsize) / (1024 * 1024));
    SPR_LOGD("Disk Usage: %d MB\n", diskUsage);
    return 0; // Return success
}

int32_t OneNetDevice::GetMemoryUsage(int32_t& memoryUsage)
{
    std::ifstream meminfo("/proc/meminfo");
    if (!meminfo.is_open()) {
        SPR_LOGE("Open /proc/meminfo failed! (%s)\n", strerror(errno));
        return false;
    }

    std::string line;
    uint64_t memTotal = 0;
    uint64_t memFree = 0;
    uint64_t buffers = 0;
    uint64_t cached = 0;

    // Read each line and parse it
    while (std::getline(meminfo, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (key == "MemTotal:") {
            iss >> memTotal;
        } else if (key == "MemFree:") {
            iss >> memFree;
        } else if (key == "Buffers:") {
            iss >> buffers;
        } else if (key == "Cached:") {
            iss >> cached;
        }
    }

    // Calculate used memory: Total - (Free + Buffers + Cached)
    // Note that this calculation gives an estimate of the memory that is not available for applications.
    uint64_t usedMemory = memTotal - (memFree + buffers + cached);
    memoryUsage = static_cast<int32_t>(usedMemory / 1024); // MB
    meminfo.close();
    SPR_LOGD("total: %llu kB, free: %llu kB, buffers: %llu kB, cached: %llu kB, used: %llu kB\n", memTotal, memFree, buffers, cached, usedMemory);

    return 0;
}

int32_t OneNetDevice::GetModelName(std::string& model)
{
    // 读取 /etc/issue 文件以获取设备型号
    std::ifstream modelFile("/etc/issue");
    if (!modelFile.is_open()) {
        return -1;
    }

    std::getline(modelFile, model);

    // 清理可能的换行符
    model.erase(std::remove(model.begin(), model.end(), '\n'), model.end());
    return 0;
}

int32_t OneNetDevice::GetLaunchTime(int32_t& launchTime)
{
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        return -1;
    }

    // 计算启动时间（单位：秒）
    launchTime = info.uptime;
    return 0;
}

int32_t OneNetDevice::GetBatteryStatus(BatteryStatus& batteryStatus)
{
    batteryStatus.percent = 0.0;
    batteryStatus.voltage = 0;
    return 0;
}

int32_t OneNetDevice::GetSystemInfo(SystemInfo& systemInfo)
{
    // 读取 /etc/os-release 文件以获取发行版信息
    std::string line;
    std::ifstream osReleaseFile("/etc/os-release");
    if (!osReleaseFile.is_open()) {
        return -1;
    }

    std::getline(osReleaseFile, line);
    while (std::getline(osReleaseFile, line)) {
        if (line.find("PRETTY_NAME=") != std::string::npos) {
            std::istringstream iss(line);
            std::string token;
            iss >> token; // Skip "PRETTY_NAME="
            std::getline(iss, token, '"'); // Read the first quoted string
            std::getline(iss, token, '"'); // Read the second quoted string
            systemInfo.description = token.empty() ? "Unknown" : token;
        } else if (line.find("VERSION_ID=") != std::string::npos) {
            std::istringstream iss(line);
            std::string token;
            iss >> token; // Skip "VERSION_ID="
            std::getline(iss, token, '"'); // Read the quoted string
            systemInfo.version = token.empty() ? "Unknown" : token;
        }
    }

    return 0;
}


/**
 * @brief Process SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT
 *
 * @param[in] msg
 * @return none
 */
void OneNetDevice::MsgRespondActiveDeviceConnect(const SprMsg& msg)
{
    int16_t devNameLen = mOneDevName.length();
    int16_t productIdLen = mOneProductID.length();
    int16_t tokenLen = mOneToken.length();

    std::string payload;
    payload.push_back((devNameLen >> 8) & 0xFF);
    payload.push_back(devNameLen & 0xFF);
    payload.append(mOneDevName);
    payload.push_back((productIdLen >> 8) & 0xFF);
    payload.push_back(productIdLen & 0xFF);
    payload.append(mOneProductID);
    payload.push_back((tokenLen >> 8) & 0xFF);
    payload.push_back(tokenLen & 0xFF);
    payload.append(mOneToken);

    SprMsg conMsg(SIG_ID_ONENET_DRV_MQTT_MSG_CONNECT);
    conMsg.SetU32Value(mKeepAliveIntervalInSec);
    conMsg.SetString(payload);
    NotifyObserver(MODULE_ONENET_DRIVER, conMsg);
}

/**
 * @brief Process SIG_ID_ONENET_MGR_DEACTIVE_DEVICE_DISCONNECT
 *
 * @param[in] msg
 * @return none
 */
void OneNetDevice::MsgRespondDeactiveDeviceDisconnect(const SprMsg& msg)
{
    SPR_LOGD("Deactive device. Reset subscribe topics\n");
    ResetAllTopics();
    SprMsg disconMsg(SIG_ID_ONENET_DRV_MQTT_MSG_DISCONNECT);
    NotifyObserver(MODULE_ONENET_DRIVER, disconMsg);
}

/**
 * @brief Process SIG_ID_ONENET_MGR_SET_CONNECT_STATUS
 *
 * @param[in] msg
 * @return none
 */
void OneNetDevice::MsgRespondSetConnectStatus(const SprMsg& msg)
{
    bool status = msg.GetBoolValue();

    SPR_LOGD("Connect status: %s\n", status ? "true" : "false");
    mConnectStatus = status;
    if (status) {
        SPR_LOGD("Device is online. Send subscribe topics\n");
        StartSubscribeTopic();
    } else {
        SPR_LOGD("Device is offline. Reset subscribe topics\n");
        ResetAllTopics();
    }
}

/**
 * @brief Process SIG_ID_ONENET_DEV_SUBSCRIBE_TOPIC
 *
 * @param[in] msg
 * @return none
 *
 * tips：主题订阅支持通配符“+”（单层）和“#”（多层），实现同时订阅多个主题消息，例如：
 *  1、订阅全部物模型相关主题：$sys/{pid}/{device-name}/thing/#
 *  2、订阅物模型属性类相关主题：$sys/{pid}/{device-name}/thing/property/#
 *  3、订阅物模型服务调用类相关主题：
 *     $sys/{pid}/{device-name}/thing/service/#
 *     或者同时订阅$sys/{pid}/{device-name}/thing/service/+/invoke、$sys/{pid}/{device-name}/thing/service/+/invoke_reply
 *  4、订阅数据流模式下的命令下发：
 *     $sys/{pid}/{device-name}/cmd/#
 *     或者同时订阅$sys/{pid}/{device-name}/cmd/request/+、$sys/{pid}/{device-name}/cmd/response/+/accepted、$sys/{pid}/{device-name}/cmd/response/+/rejected
 *     订阅全部物模型相关主题：$sys/{pid}/{device-name}/thing/#
 */
void OneNetDevice::MsgRespondSubscribeTopic(const SprMsg& msg)
{
    std::string tmpTopic;
    auto topicIt = std::find_if(mAllTopicMap.begin(), mAllTopicMap.end(),
        [](const std::pair<std::string, OneNetTopic>& topic) {
        return topic.second.status == TOPIC_STATUS_IDLE;
    });

    if (topicIt == mAllTopicMap.end()) {
        SPR_LOGD("All topics are subscribed\n");
        return;
    }

    uint16_t identity = GetUnusedIdentity();
    tmpTopic = topicIt->second.topic;
    topicIt->second.status = TOPIC_STATUS_WAIT_ACK;
    topicIt->second.identifier = identity;
    mCurTopic = tmpTopic;
    SPR_LOGD("subscribe topic: %s\n", tmpTopic.c_str());

    SprMsg tmpMsg(SIG_ID_ONENET_DRV_MQTT_MSG_SUBSCRIBE);
    tmpMsg.SetU16Value(identity);
    tmpMsg.SetString(tmpTopic);
    NotifyObserver(MODULE_ONENET_DRIVER, tmpMsg);
}

/**
 * @brief Process SIG_ID_ONENET_DRV_MQTT_MSG_SUBACK
 *
 * @param[in] msg
 * @return none
 */
void OneNetDevice::MsgRespondSubscribeTopicAck(const SprMsg& msg)
{
    uint8_t retCode = msg.GetU8Value();
    uint16_t identity = msg.GetU16Value();
    auto topicIt = std::find_if(mAllTopicMap.begin(), mAllTopicMap.end(),
        [identity](const std::pair<std::string, OneNetTopic>& topic) {
        return topic.second.identifier == identity;
    });

    if (topicIt != mAllTopicMap.end()) {
        topicIt->second.status = (retCode != 0x80) ? TOPIC_STATUS_SUBSCRIBED_SUCCESS : TOPIC_STATUS_SUBSCRIBING_FAIL;
    }

    SPR_LOGD("Recv subscribe topic [%s] ack, id: %02X ret: %02X \n", mCurTopic.c_str(), identity, retCode);
    ReleaseIdentity(identity);
    SprMsg tMsg(SIG_ID_ONENET_DEV_SUBSCRIBE_TOPIC);
    SendMsg(tMsg);
}

/**
 * @brief Process SIG_ID_ONENET_MGR_PING_TIMER_EVENT
 *
 * @param[in] msg
 * @return none
 */
void OneNetDevice::MsgRespondPingTimerEvent(const SprMsg& msg)
{
    SprMsg pingMsg(SIG_ID_ONENET_DRV_MQTT_MSG_PINGREQ);
    NotifyObserver(MODULE_ONENET_DRIVER, pingMsg);
}

/**
 * @brief Process SIG_ID_ONENET_MGR_DATA_REPORT_TIMER_EVENT
 *
 * @param[in] msg
 * @return none
 */
void OneNetDevice::MsgRespondDataReportTimerEvent(const SprMsg& msg)
{
    char topicThingPost[100] = {0};
    std::string bodyJson = PreparePublishPayloadJson();
    uint16_t identity = GetUnusedIdentity();
    snprintf(topicThingPost, sizeof(topicThingPost), TEMPLATE_TOPIC_THING_POST, mOneProductID.c_str(), mOneDevName.c_str());

    auto pParam = std::make_shared<SOneNetPublishParam>();
    pParam->flags = 0x00;
    pParam->identifier = identity;
    strncpy(pParam->topic, topicThingPost, sizeof(pParam->topic) - 1);
    strncpy(pParam->payload, (char*)bodyJson.c_str(), sizeof(pParam->payload) - 1);

    SprMsg tMsg(SIG_ID_ONENET_DRV_MQTT_MSG_PUBLISH);
    tMsg.SetDatas(pParam, sizeof(SOneNetPublishParam));
    NotifyObserver(MODULE_ONENET_DRIVER, tMsg);
}

int32_t OneNetDevice::ProcessMsg(const SprMsg& msg)
{
    SPR_LOGD("Recv msg: %s\n", GetSigName(msg.GetMsgId()));
    switch (msg.GetMsgId()) {
        case SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT: {
            MsgRespondActiveDeviceConnect(msg);
            break;
        }
        case SIG_ID_ONENET_MGR_DEACTIVE_DEVICE_DISCONNECT: {
            MsgRespondDeactiveDeviceDisconnect(msg);
            break;
        }
        case SIG_ID_ONENET_MGR_SET_CONNECT_STATUS: {
            MsgRespondSetConnectStatus(msg);
            break;
        }
        case SIG_ID_ONENET_DEV_SUBSCRIBE_TOPIC: {
            MsgRespondSubscribeTopic(msg);
            break;
        }
        case SIG_ID_ONENET_DRV_MQTT_MSG_SUBACK: {
            MsgRespondSubscribeTopicAck(msg);
            break;
        }
        case SIG_ID_ONENET_MGR_PING_TIMER_EVENT: {
            MsgRespondPingTimerEvent(msg);
            break;
        }
        case SIG_ID_ONENET_MGR_DATA_REPORT_TIMER_EVENT: {
            MsgRespondDataReportTimerEvent(msg);
            break;
        }
        default: {
            break;
        }
    }
    return 0;
}
