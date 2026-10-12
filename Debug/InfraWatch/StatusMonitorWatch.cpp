/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : StatusMonitorWatch.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/10/12
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <sstream>
#include <string>
#include <vector>
#include "InfraCommon.h"
#include "CommonErrorCodes.h"
#include "SprEnumHelper.h"
#include "StatusMonitorInterface.h"
#include "StatusMonitorWatch.h"

using namespace InternalDefs;

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

StatusMonitorWatch& StatusMonitorWatch::GetInstance() {
    static StatusMonitorWatch instance;
    return instance;
}

void StatusMonitorWatch::Usage() {
    InfraWatch::ClearScreen();
    SPR_LOG("===============================  Status Monitor  ================================\n"
            "\n"
            "    1. GetAllEvents \n"
            "    2. GetEvent     \n"
            "\n"
            "    [Q] Quit\n"
            "\n"
            "=================================================================================\n");
}

void StatusMonitorWatch::Menu(char input) {
    switch(input) {
        case '1': {
            HandleGetAllEvents();
            break;
        }
        case '2': {
            HandleGetEvent();
            break;
        }
        case 'q': {
            break;
        }
        default:
            break;
    }
}

void StatusMonitorWatch::PrintEvents(const std::vector<StatusEvent>& events) {
    InfraWatch::ClearScreen();
    SPR_LOG("                        Show Status Events (%d)                            \n", (int)events.size());
    SPR_LOG("-----------------------------------------------------------------------------------------------\n");
    SPR_LOG("MODULE    LEVEL STATUSCODE         TIME    TEXT                                                \n");
    SPR_LOG("-----------------------------------------------------------------------------------------------\n");

    for (std::vector<StatusEvent>::const_iterator it = events.begin(); it != events.end(); ++it) {
        int32_t level = GetSprErrorLevel(it->sErrorCode);
        SPR_LOG("%6d %8d %10d %12ld    %s\n",
                it->sModuleID,
                level,
                it->sErrorCode,
                (long)it->sTime,
                it->sText.c_str());
    }

    SPR_LOG("-----------------------------------------------------------------------------------------------\n");
}

int StatusMonitorWatch::HandleGetAllEvents() {
    std::vector<StatusEvent> events;
    int ret = StatusMonitorInterface::GetInstance()->GetAllEvents(events);
    if (ret != ERR_GENERAL_SUCCESS) {
        SPR_LOG("GetAllEvents failed!\n");
        return -1;
    }

    PrintEvents(events);
    InfraWatch::WaitUserInputWithoutEnter("Press any key to continue...");
    return 0;
}

int StatusMonitorWatch::HandleGetEvent() {
    std::string line;
    std::string extra;
    int32_t moduleID = 0;
    int32_t level = ERR_EVENT_LEVEL_UNKNOWN;
    int32_t errCode = ERR_GENERAL_SUCCESS;

    SPR_LOG("Input filter <moduleID level errCode>\n");
    if (InfraWatch::ReadLineFromUserInput(line) != 0) {
        SPR_LOG("Invalid input!\n");
        return -1;
    }

    if (!line.empty()) {
        std::istringstream iss(line);
        if (!(iss >> moduleID >> level >> errCode) || (iss >> extra)) {
            SPR_LOG("Invalid input!\n");
            return -1;
        }
    }

    std::vector<StatusEvent> events;
    int ret = StatusMonitorInterface::GetInstance()->GetEvent(events, moduleID, level, errCode);
    if (ret != ERR_GENERAL_SUCCESS) {
        SPR_LOG("GetEvent failed!\n");
        return -1;
    }

    PrintEvents(events);
    InfraWatch::WaitUserInputWithoutEnter("Press any key to continue...");
    return 0;
}
