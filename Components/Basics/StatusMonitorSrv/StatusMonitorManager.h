/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : StatusMonitorManager.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/02/20
 *
 *  The minimum scale of the timer is milliseconds, and the value set during use must be
 *  a multiple of 1 milliseconds
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/02/20 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __STATUS_MONITOR_MANAGER_H__
#define __STATUS_MONITOR_MANAGER_H__

#include <list>
#include <time.h>
#include "SprObserverWithMQueue.h"

struct StatusEvent
{
    time_t sTime;
    int32_t sErrorCode;
    std::string sText;

    StatusEvent(time_t time, int32_t errorCode, const std::string& text)
        : sTime(time), sErrorCode(errorCode), sText(text) {}
};

class StatusMonitorManager : public SprObserverWithMQueue
{
public:
    StatusMonitorManager(ModuleIDType id, const std::string& name);
    ~StatusMonitorManager();

protected:
    int32_t Init() override;
    int32_t ProcessMsg(const SprMsg& msg) override;

private:
    int32_t InitDebugNode();
    int32_t AddStatusEvent(uint32_t moduleID, int32_t errCode, const std::string& text);
    int32_t DumpStatusEventsWithFilter(int32_t moduleID, int32_t level, int32_t errCode, const std::string& text);
    int32_t DumpAllStatusEvents();
    int32_t DumpStatusEventsWithModuleID(int32_t moduleID);
    int32_t DumpStatusEventsWithLevel(int32_t level);
    int32_t DumpStatusEventsWithErrorCode(int32_t errCode);
    int32_t DumpStatusEventsWithText(const std::string& text);
    int32_t DelStatusEventWithFilter(int32_t moduleID, int32_t level, int32_t errCode, const std::string& text);
    int32_t DelAllStatusEvents();
    int32_t DelStatusEventsWithModuleID(int32_t moduleID);
    int32_t DelStatusEventsWithLevel(int32_t level);
    int32_t DelStatusEventsWithErrorCode(int32_t errCode);
    int32_t DelStatusEventsWithText(const std::string& text);
    std::string FormatTimeAsLocalString(const time_t& time);

    /* 注册/注销所有调试函数 */
    void RegisterDebugFuncs();
    void UnregisterDebugFuncs();

    /* 调试函数 */
    void DebugDisplayUTCTimeAsLocalString(const std::vector<std::string>& args);
    void DebugDumpAllStatusEvents(const std::vector<std::string>& args);
    void DebugDumpStatusEventsWithModuleID(const std::vector<std::string>& args);
    void DebugDumpStatusEventsWithLevel(const std::vector<std::string>& args);
    void DebugDumpStatusEventsWithErrorCode(const std::vector<std::string>& args);
    void DebugDumpStatusEventsWithText(const std::vector<std::string>& args);
    void DebugDelAllStatusEvent(const std::vector<std::string>& args);
    void DebugDelStatusEventsWithModuleID(const std::vector<std::string>& args);
    void DebugDelStatusEventsWithLevel(const std::vector<std::string>& args);
    void DebugDelStatusEventsWithErrorCode(const std::vector<std::string>& args);
    void DebugDelStatusEventsWithText(const std::vector<std::string>& args);
    void DebugAddStatusEvent(const std::vector<std::string>& args);

private:
    int32_t mErasedNum;  // 已删除的记录数
    std::map<int32_t, std::list<StatusEvent>> mAllEvents;  // key: moduleID, value: events
};

#endif // __STATUS_MONITOR_MANAGER_H__
