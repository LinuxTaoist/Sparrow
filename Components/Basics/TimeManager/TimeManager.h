/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TimeManager.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/11/11
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/11/11 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __TIME_MANAGER_H__
#define __TIME_MANAGER_H__

#include "NtpSource.h"
#include "SprObserverWithMQueue.h"

enum TimeSourcePriority
{
    TIME_SOURCE_PRIORITY_NONE,
    TIME_SOURCE_PRIORITY_HIGHEST,
    TIME_SOURCE_PRIORITY_HIGH,
    TIME_SOURCE_PRIORITY_MEDIUM,
    TIME_SOURCE_PRIORITY_LOW,
    TIME_SOURCE_PRIORITY_LOWEST,
    TIME_SOURCE_PRIORITY_BUTT
};

class TimeManager : public SprObserverWithMQueue
{
public:
    /**
     * @brief Get the single instance
     *
     * @param id
     * @param name
     * @return TimeManager*
     */
    static TimeManager* GetInstance(ModuleIDType id, const std::string& name);

    int32_t StartSyncTimePoller();
    int32_t StopSyncTimePoller();

private:
    /**
     * @brief Constructor / Destructor
     *
     * @param[in] id
     * @param[in] name
     */
    TimeManager(ModuleIDType id, const std::string& name);
    virtual ~TimeManager();

     /**
     * @brief Initializes the business module with overrides from derived business modules
     *
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t Init() override;

    /**
     * @brief private functions
     */
    int32_t InitNtpSource();
    int32_t RequestNtpTime();
    int32_t RequestGnssTime();
    int32_t StartSyncTime();
    int32_t SmoothAdjustSystemTime(int64_t ns);
    int32_t JumpAdjustSystemTime(uint64_t timestamp);
    int32_t SyncSystemTime(int32_t source, uint64_t timestamp);
    int32_t GetDiffWithLocalTime(uint64_t timestamp, int64_t& diff);

    TimeSourcePriority GetTimeSourcePriority(InternalDefs::TimeSourceType source);
    InternalDefs::TimeSourceType GetTimeSource(TimeSourcePriority priority);

    /**
     * @brief  Process message from message queue received
     *
     * @param msg
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t ProcessMsg(const SprMsg& msg) override;

    /**
     * @brief message handle function
     *
     * @param[in] msg
     */
    void MsgRespondSyncTimeTimerEvent(const SprMsg& msg);
    void MsgRespondSyncSystemTime(const SprMsg& msg);
    void MsgRespondSyncTimePollerTimerEvent(const SprMsg& msg);
    void MsgRespondPowerStartupHigh(const SprMsg& msg);
    void MsgRespondPowerStandbyLow(const SprMsg& msg);

    /**
     * @brief Register / Unregister debug functions
     */
    void RegisterDebugFuncs();
    void UnregisterDebugFuncs();

    /** Debug functions */
    void DebugDumpDetails(const std::vector<std::string>& args);
    void DebugStartSyncTime(const std::vector<std::string>& args);
    void DebugStartSyncTimePoller(const std::vector<std::string>& args);
    void DebugStopSyncTimePoller(const std::vector<std::string>& args);
    void DebugRequestNtpTime(const std::vector<std::string>& args);
    void DebugSetSyncTimeOutMs(const std::vector<std::string>& args);
    void DebugSetSyncPollTimeOutMs(const std::vector<std::string>& args);

private:
    bool mSyncPollerTimer;
    bool mSyncTimeFinished;
    int32_t mReqPriority;
    int32_t mSyncTimeOutMs;
    int32_t mSyncPollTimeOutMs;
    InternalDefs::TimeSourceType mCurTimeSource;
    std::shared_ptr<NtpSource> mpNtpSource;
    std::map<TimeSourcePriority, InternalDefs::TimeSourceType> mTimeSourceMap; // key: priority, value: source

    using DebugCmdFunc = void (TimeManager::*)(const std::string&);
    std::map<std::string, std::pair<std::string, DebugCmdFunc>> mDebugCmdMap;
};

#endif // __TIME_MANAGER_H__
