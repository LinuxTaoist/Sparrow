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

#include "NtpClient.h"
#include "SprObserverWithMQueue.h"

enum TimeSourcePriority
{
    TIME_SOURCE_PRIORITY_HIGHEST = 0,
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
     * @brief Destructor
     *
     */
    virtual ~TimeManager();

    /**
     * @brief  Process message from message queue received
     *
     * @param msg
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t ProcessMsg(const SprMsg& msg) override;

    /**
     * @brief Get the single instance
     *
     * @param id
     * @param name
     * @return TimeManager*
     */
    static TimeManager* GetInstance(ModuleIDType id, const std::string& name);

private:
    /**
     * @brief Constructor
     *
     * @param[in] id
     * @param[in] name
     */
    TimeManager(ModuleIDType id, const std::string& name);

    int32_t InitDebugDetails();
    int32_t RequestNtpTime();
    int32_t RequestGnssTime();

private:
    TimeSourcePriority mCurPriority;
    InternalDefs::TimeSourceType mCurTimeSource;
    std::shared_ptr<NtpClient> mNtpCliPtr;
    std::map<TimeSourcePriority, InternalDefs::TimeSourceType> mTimeSourceMap;

    using DebugCmdFunc = void (TimeManager::*)(const std::string&);
    std::map<std::string, std::pair<std::string, DebugCmdFunc>> mDebugCmdMap;
};

#endif // __TIME_MANAGER_H__
