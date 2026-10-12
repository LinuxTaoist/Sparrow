/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : StatusMonitorHub.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/10/12
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "SprLog.h"
#include "CommonMacros.h"
#include "CommonErrorCodes.h"
#include "CoreTypeDefs.h"
#include "StatusMonitorHub.h"

using namespace InternalDefs;

#define LOG_TAG "MonHub"

StatusMonitorHub::StatusMonitorHub(const std::string& srvName, StatusMonitorManager* statusMonitorManager)
    : BinderHub(srvName) {
    mStatusMonitorManager = statusMonitorManager;
}

StatusMonitorHub::~StatusMonitorHub() {
}

void StatusMonitorHub::handleCmd(const std::shared_ptr<Parcel>& pReqParcel, const std::shared_ptr<Parcel>& pRspParcel, int cmd) {
    switch (cmd) {
        case STATUSMON_CMD_GET_ALL_EVENTS: {
            std::vector<StatusEvent> events;
            int32_t ret = mStatusMonitorManager->GetAllStatusEvents(events);
            NONZERO_CHECK(pRspParcel->WriteInt(ret));
            NONZERO_CHECK(pRspParcel->WriteInt((int32_t)events.size()));
            for (const auto& event : events) {
                NONZERO_CHECK(pRspParcel->WriteInt(event.sModuleID));
                NONZERO_CHECK(pRspParcel->WriteInt(event.sErrorCode));
                NONZERO_CHECK(pRspParcel->WriteString(event.sText));
                NONZERO_CHECK(pRspParcel->WriteData((void*)&event.sTime, sizeof(event.sTime)));
            }
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        case STATUSMON_CMD_GET_EVENT: {
            int32_t moduleID = 0;
            int32_t level = ERR_EVENT_LEVEL_UNKNOWN;
            int32_t errCode = ERR_GENERAL_SUCCESS;
            int32_t ret = 0;

            if (pReqParcel->ReadInt(moduleID) != 0 ||
                pReqParcel->ReadInt(level) != 0 ||
                pReqParcel->ReadInt(errCode) != 0) {
                SPR_LOGE("Read filter failed!\n");
                NONZERO_CHECK(pRspParcel->WriteInt(-1));
                NONZERO_CHECK(pRspParcel->Post());
                break;
            }

            std::vector<StatusEvent> events;
            ret = mStatusMonitorManager->GetStatusEventsWithFilter(moduleID, level, errCode, events);
            NONZERO_CHECK(pRspParcel->WriteInt(ret));
            NONZERO_CHECK(pRspParcel->WriteInt((int32_t)events.size()));
            for (const auto& event : events) {
                NONZERO_CHECK(pRspParcel->WriteInt(event.sModuleID));
                NONZERO_CHECK(pRspParcel->WriteInt(event.sErrorCode));
                NONZERO_CHECK(pRspParcel->WriteString(event.sText));
                NONZERO_CHECK(pRspParcel->WriteData((void*)&event.sTime, sizeof(event.sTime)));
            }
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        default: {
            SPR_LOGE("Unknown cmd: 0x%x\n", cmd);
            break;
        }
    }
}
