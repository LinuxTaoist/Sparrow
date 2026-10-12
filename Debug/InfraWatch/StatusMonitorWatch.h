/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : StatusMonitorWatch.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/10/12
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __STATUS_MONITOR_WATCH_H__
#define __STATUS_MONITOR_WATCH_H__

#include <vector>
#include "BaseWatch.h"
#include "StatusMonitorInterface.h"

class StatusMonitorWatch : public BaseWatch {
public:
    static StatusMonitorWatch& GetInstance();

private:
    StatusMonitorWatch() = default;
    ~StatusMonitorWatch() override = default;
    void Usage() override;
    void Menu(char input) override;
    void PrintEvents(const std::vector<StatusEvent>& events);
    int HandleGetAllEvents();
    int HandleGetEvent();
};

#endif // __STATUS_MONITOR_WATCH_H__
