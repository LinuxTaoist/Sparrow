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
#include "SprLog.h"
#include "SprDebugNode.h"
#include "CommonMacros.h"
#include "CommonErrorCodes.h"
#include "SprEnumHelper.h"
#include "StatusMonitorManager.h"

using namespace InternalDefs;

#define LOG_TAG "MonitorM"

#define OWNER_STATUSMONITOR         "StatusMonitorManager"
#define STATUS_EVENT_NUM_LIMIT      100

StatusMonitorManager::StatusMonitorManager(ModuleIDType id, const std::string& name)
            : SprObserverWithMQueue(id, name), mErasedNum(0)
{
}

StatusMonitorManager::~StatusMonitorManager()
{
    UnregisterDebugFuncs();
}

int32_t StatusMonitorManager::Init()
{
    RegisterDebugFuncs();
    return 0;
}

int32_t StatusMonitorManager::AddStatusEvent(uint32_t moduleID, int32_t errCode, const std::string& text)
{
    auto& event = mAllEvents[moduleID];
    time_t now = time(nullptr);

    if (event.size() >= STATUS_EVENT_NUM_LIMIT) {
        event.pop_front();
        mErasedNum++;
    }

    event.push_back({now, errCode, text});
    return 0;
}

int32_t StatusMonitorManager::DumpStatusEventsWithFilter(int32_t moduleID, int32_t level, int32_t errCode, const std::string& text)
{
    int32_t totalNum = mErasedNum + mAllEvents.size();
    SPR_LOGI("                             Show All Status Events (%d)                                       \n", totalNum);
    SPR_LOGI("-----------------------------------------------------------------------------------------------\n");
    SPR_LOGI("MODULE    LEVEL STATUSCODE         TIME    TEXT                                                \n");
    SPR_LOGI("-----------------------------------------------------------------------------------------------\n");
    for (const auto& moduleEvents : mAllEvents) {
        int32_t tmpID = moduleEvents.first;
        auto& tmpEvents = moduleEvents.second;

        for (auto it = tmpEvents.rbegin(); it != tmpEvents.rend(); ++it) {
            auto& event = *it;
            int32_t tmpLevel = GetSprErrorLevel(event.sErrorCode);

            if ( (moduleID == 0 || moduleID == tmpID)                               &&
                 (level == ERR_EVENT_LEVEL_UNKNOWN || level == tmpLevel)            &&
                 (errCode == ERR_GENERAL_SUCCESS || errCode == event.sErrorCode)    &&
                 (text.empty() || text == event.sText.c_str()) ) {

                std::string timeStr = FormatTimeAsLocalString(event.sTime);
                SPR_LOGI("%6d %8d %10d %12d    %s %s", tmpID, tmpLevel, event.sErrorCode, event.sTime, timeStr.c_str(), event.sText.c_str());
            }
        }
    }

    SPR_LOGI("-----------------------------------------------------------------------------------------------\n");
    return 0;
}

int32_t StatusMonitorManager::DumpAllStatusEvents()
{
    DumpStatusEventsWithFilter(0, ERR_EVENT_LEVEL_UNKNOWN, ERR_GENERAL_SUCCESS, "");
    return 0;
}

int32_t StatusMonitorManager::DumpStatusEventsWithModuleID(int32_t moduleID)
{
    DumpStatusEventsWithFilter(moduleID, ERR_EVENT_LEVEL_UNKNOWN, ERR_GENERAL_SUCCESS, "");
    return 0;
}

int32_t StatusMonitorManager::DumpStatusEventsWithLevel(int32_t level)
{
    DumpStatusEventsWithFilter(0, level, ERR_GENERAL_SUCCESS, "");
    return 0;
}

int32_t StatusMonitorManager::DumpStatusEventsWithErrorCode(int32_t errCode)
{
    DumpStatusEventsWithFilter(0, ERR_EVENT_LEVEL_UNKNOWN, errCode, "");
    return 0;
}

int32_t StatusMonitorManager::DumpStatusEventsWithText(const std::string& text)
{
    DumpStatusEventsWithFilter(0, ERR_EVENT_LEVEL_UNKNOWN, ERR_GENERAL_SUCCESS, text);
    return 0;
}

int32_t StatusMonitorManager::DelStatusEventWithFilter(int32_t moduleID, int32_t level, int32_t errCode, const std::string& text)
{
    for (auto& moduleEvents : mAllEvents) {
        int32_t tmpModuleID = moduleEvents.first;
        auto& tmpEvents = moduleEvents.second;

        for (auto it = tmpEvents.begin(); it != tmpEvents.end(); ) {
            int32_t tmpErrCode = (*it).sErrorCode;
            int32_t tmpLevel = GetSprErrorLevel(tmpErrCode);
            std::string tmpText = (*it).sText;

            bool shouldDelete = (moduleID == 0 || moduleID == tmpModuleID) &&
                                (level == ERR_EVENT_LEVEL_UNKNOWN || level == tmpLevel) &&
                                (errCode == ERR_GENERAL_SUCCESS || errCode == tmpErrCode) &&
                                (text.empty() || text == tmpText);

            if (shouldDelete) {
                it = tmpEvents.erase(it);
            } else {
                ++it;
            }
        }
    }

    return 0;
}

int32_t StatusMonitorManager::DelAllStatusEvents()
{
    mAllEvents.clear();
    return 0;
}

int32_t StatusMonitorManager::DelStatusEventsWithModuleID(int32_t moduleID)
{
    DelStatusEventWithFilter(moduleID, ERR_EVENT_LEVEL_UNKNOWN, ERR_GENERAL_SUCCESS, "");
    return 0;
}

int32_t StatusMonitorManager::DelStatusEventsWithLevel(int32_t level)
{
    DelStatusEventWithFilter(0, level, ERR_GENERAL_SUCCESS, "");
    return 0;
}

int32_t StatusMonitorManager::DelStatusEventsWithErrorCode(int32_t errCode)
{
    DelStatusEventWithFilter(0, ERR_EVENT_LEVEL_UNKNOWN, errCode, "");
    return 0;
}

int32_t StatusMonitorManager::DelStatusEventsWithText(const std::string& text)
{
    DelStatusEventWithFilter(0, ERR_EVENT_LEVEL_UNKNOWN, ERR_GENERAL_SUCCESS, text);
    return 0;
}

std::string StatusMonitorManager::FormatTimeAsLocalString(const time_t& time)
{
    struct tm* pTime = localtime(&time);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", pTime);
    return buffer;
}

int32_t StatusMonitorManager::ProcessMsg(const SprMsg& msg)
{
    SPR_LOGD("Recv msg: %s\n", GetSigName(msg.GetMsgId()));
    switch (msg.GetMsgId()) {
        case SIG_ID_MONITOR_STATUS_EVENT: {
            SPR_LOGI("Receive status event!\n");
            AddStatusEvent(msg.GetFrom(), msg.GetI32Value(), msg.GetString());
            break;
        }
        default: {
            break;
        }
    }

    return 0;
}

void StatusMonitorManager::RegisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    p->RegisterCmd(OWNER_STATUSMONITOR, "DisplayUTC",       "Display UTC",      std::bind(&StatusMonitorManager::DebugDisplayUTCTimeAsLocalString, this, std::placeholders::_1));
    p->RegisterCmd(OWNER_STATUSMONITOR, "DumpAllEvents",    "Dump All Events",  std::bind(&StatusMonitorManager::DebugDumpAllStatusEvents, this, std::placeholders::_1));
    p->RegisterCmd(OWNER_STATUSMONITOR, "DumpWithID",       "Dump With ID",     std::bind(&StatusMonitorManager::DebugDumpStatusEventsWithModuleID, this, std::placeholders::_1));
    p->RegisterCmd(OWNER_STATUSMONITOR, "DumpWithLevel",    "Dump With Level",  std::bind(&StatusMonitorManager::DebugDumpStatusEventsWithLevel, this, std::placeholders::_1));
    p->RegisterCmd(OWNER_STATUSMONITOR, "DumpWithErrCode",  "Dump With ErrCode",std::bind(&StatusMonitorManager::DebugDumpStatusEventsWithErrorCode, this, std::placeholders::_1));
    p->RegisterCmd(OWNER_STATUSMONITOR, "DumpWithText",     "Dump With Text",   std::bind(&StatusMonitorManager::DebugDumpStatusEventsWithText, this, std::placeholders::_1));
    p->RegisterCmd(OWNER_STATUSMONITOR, "AddStatusEvent",   "Add Status Event", std::bind(&StatusMonitorManager::DebugAddStatusEvent, this, std::placeholders::_1));
    p->RegisterCmd(OWNER_STATUSMONITOR, "DelAllEvents",     "Del All Events",   std::bind(&StatusMonitorManager::DebugDelAllStatusEvent, this, std::placeholders::_1));
    p->RegisterCmd(OWNER_STATUSMONITOR, "DelWithID",        "Del With ID",      std::bind(&StatusMonitorManager::DebugDelStatusEventsWithModuleID, this, std::placeholders::_1));
    p->RegisterCmd(OWNER_STATUSMONITOR, "DelWithLevel",     "Del With Level",   std::bind(&StatusMonitorManager::DebugDelStatusEventsWithLevel, this, std::placeholders::_1));
    p->RegisterCmd(OWNER_STATUSMONITOR, "DelWithErrCode",   "Del With ErrCode", std::bind(&StatusMonitorManager::DebugDelStatusEventsWithErrorCode, this, std::placeholders::_1));
    p->RegisterCmd(OWNER_STATUSMONITOR, "DelWithText",      "Del With Text",    std::bind(&StatusMonitorManager::DebugDelStatusEventsWithText, this, std::placeholders::_1));
}

void StatusMonitorManager::UnregisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    SPR_LOGD("Unregister %s all debug funcs\n", mModuleName.c_str());
    p->UnregisterCmd(OWNER_STATUSMONITOR);
}

void StatusMonitorManager::DebugDisplayUTCTimeAsLocalString(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        SPR_LOGE("Invalid args! size = %d\n", args.size());
        SPR_LOGE("Usage: echo DisplayUTCTimeAsLocalString {utc} > %s/statusmonitorsrv\n", DEFAULT_DEBUG_ROOT_DIR);
        return;
    }

    time_t utc = atoi(args[1].c_str());
    if (utc == 0) {
        SPR_LOGE("Invalid utc: %s\n", args[1].c_str());
        return;
    }

    SPR_LOGI("utc: %u, local: %s", utc, ctime(&utc));
}

void StatusMonitorManager::DebugDumpAllStatusEvents(const std::vector<std::string>& args)
{
    SPR_LOGI("Dump all events\n");
    DumpAllStatusEvents();
}

void StatusMonitorManager::DebugDumpStatusEventsWithModuleID(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        SPR_LOGE("Invalid args! size = %d\n", args.size());
        SPR_LOGE("Usage: echo DumpWithID {moduleID} > %s/statusmonitorsrv\n", DEFAULT_DEBUG_ROOT_DIR);
        return;
    }

    int32_t moduleID = atoi(args[1].c_str());
    if (moduleID == 0) {
        SPR_LOGE("Invalid moduleID: %s\n", args[1].c_str());
        return;
    }

    SPR_LOGI("Dump event moduleID: %d\n", moduleID);
    DumpStatusEventsWithModuleID(moduleID);
}

void StatusMonitorManager::DebugDumpStatusEventsWithLevel(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        SPR_LOGE("Invalid args! size = %d\n", args.size());
        SPR_LOGE("Usage: echo DumpWithLevel {level} > %s/statusmonitorsrv\n", DEFAULT_DEBUG_ROOT_DIR);
        return;
    }

    int32_t level = atoi(args[1].c_str());
    if (level == 0) {
        SPR_LOGE("Invalid level: %s\n", args[1].c_str());
        return;
    }

    SPR_LOGI("Dump event level: %d\n", level);
    DumpStatusEventsWithLevel(level);
}

void StatusMonitorManager::DebugDumpStatusEventsWithErrorCode(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        SPR_LOGE("Invalid args! size = %d\n", args.size());
        SPR_LOGE("Usage: echo DumpWithErrCode {errCode} > %s/statusmonitorsrv\n", DEFAULT_DEBUG_ROOT_DIR);
        return;
    }

    int32_t errCode = atoi(args[1].c_str());
    if (errCode == 0) {
        SPR_LOGE("Invalid errCode: %s\n", args[1].c_str());
        return;
    }

    SPR_LOGI("Dump error code: %d\n", errCode);
    DumpStatusEventsWithErrorCode(errCode);
}

void StatusMonitorManager::DebugDumpStatusEventsWithText(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        SPR_LOGE("Invalid args! size = %d\n", args.size());
        SPR_LOGE("Usage: echo DumpWithText {text} > %s/statusmonitorsrv\n", DEFAULT_DEBUG_ROOT_DIR);
        return;
    }

    SPR_LOGI("Dump error content: %s\n", args[1].c_str());
    DumpStatusEventsWithText(args[1]);
}

void StatusMonitorManager::DebugDelAllStatusEvent(const std::vector<std::string>& args)
{
    SPR_LOGI("Delete all events\n");
    DelAllStatusEvents();
}

void StatusMonitorManager::DebugDelStatusEventsWithModuleID(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        SPR_LOGE("Invalid args! size = %d\n", args.size());
        SPR_LOGE("Usage: echo DelWithID {moduleID} > %s/statusmonitorsrv\n", DEFAULT_DEBUG_ROOT_DIR);
        return;
    }

    int32_t moduleID = atoi(args[1].c_str());
    if (moduleID == 0) {
        SPR_LOGE("Invalid moduleID: %s\n", args[1].c_str());
        return;
    }

    SPR_LOGI("Delete event moduleID: %d\n", moduleID);
    DelStatusEventsWithModuleID(moduleID);
}

void StatusMonitorManager::DebugDelStatusEventsWithLevel(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        SPR_LOGE("Invalid args! size = %d\n", args.size());
        SPR_LOGE("Usage: echo DelWithLevel {level} > %s/statusmonitorsrv\n", DEFAULT_DEBUG_ROOT_DIR);
        return;
    }

    int32_t level = atoi(args[1].c_str());
    if (level == 0) {
        SPR_LOGE("Invalid level: %s\n", args[1].c_str());
        return;
    }

    SPR_LOGI("Delete event level: %d\n", level);
    DelStatusEventsWithLevel(level);
}

void StatusMonitorManager::DebugDelStatusEventsWithErrorCode(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        SPR_LOGE("Invalid args! size = %d\n", args.size());
        SPR_LOGE("Usage: echo DelWithErrCode {errCode} > %s/statusmonitorsrv\n", DEFAULT_DEBUG_ROOT_DIR);
        return;
    }

    int32_t errCode = atoi(args[1].c_str());
    if (errCode == 0) {
        SPR_LOGE("Invalid errCode: %s\n", args[1].c_str());
        return;
    }

    SPR_LOGI("Delete event errCode: %d\n", errCode);
    DelStatusEventsWithErrorCode(errCode);
}

void StatusMonitorManager::DebugDelStatusEventsWithText(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        SPR_LOGE("Invalid args! size = %d\n", args.size());
        SPR_LOGE("Usage: echo DelWithText {text} > %s/statusmonitorsrv\n", DEFAULT_DEBUG_ROOT_DIR);
        return;
    }

    SPR_LOGI("Delete event text: %s\n", args[1].c_str());
    DelStatusEventsWithText(args[1]);
}

void StatusMonitorManager::DebugAddStatusEvent(const std::vector<std::string>& args)
{
    SprMsg msg(SIG_ID_MONITOR_STATUS_EVENT);
    msg.SetU32Value(ERR_GENERAL_ERROR);
    msg.SetString("For test");
    SendMsg(msg);
}
