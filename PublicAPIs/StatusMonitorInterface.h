/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : StatusMonitorInterface.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/10/12
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __STATUS_MONITOR_INTERFACE_H__
#define __STATUS_MONITOR_INTERFACE_H__

#include <string>
#include <vector>
#include <stdint.h>
#include "CommonTypeDefs.h"
#include "CommonErrorCodes.h"

class StatusMonitorInterface {
public:
    /**
     * @brief Get the Instance object
     *
     * @return Single instance of StatusMonitorInterface
     */
    static StatusMonitorInterface* GetInstance();

    /**
     * @brief  Get all status events
     *
     * @param events Output event list
     * @return  0 if success, -1 if failed
     */
    int GetAllEvents(std::vector<StatusEvent>& events);

    /**
     * @brief  Get system event list with filter
     *
     *         moduleID, level and errCode support default values. When a value equals the default
     *         meaning, it will match all events for that condition.
     *
     * @param events Output event list
     * @param moduleID Filter by moduleID, 0 for all
     * @param level Filter by event level, ERR_EVENT_LEVEL_UNKNOWN for all
     * @param errCode Filter by errCode, ERR_GENERAL_SUCCESS for all
     * @return  0 if success, -1 if failed
     */
    int GetEvent(std::vector<StatusEvent>& events,
                 int32_t moduleID = 0,
                 int32_t level = ERR_EVENT_LEVEL_UNKNOWN,
                 int32_t errCode = ERR_GENERAL_SUCCESS);

private:
    /**
     * @brief Constructor
     */
    StatusMonitorInterface();

    /**
     * @brief Destructor
     */
    ~StatusMonitorInterface();
};

#endif // __STATUS_MONITOR_INTERFACE_H__
