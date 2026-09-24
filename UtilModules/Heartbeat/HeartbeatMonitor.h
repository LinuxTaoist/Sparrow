/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : HeartbeatMonitor.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Heartbeat monitor interface.
 *  @date       : 2026/09/17
 *
 *  Usage:
 *      HeartbeatMonitor monitor;
 *      monitor.Start(
 *          HEARTBEAT_DEFAULT_CHANNEL_PATH,
 *          HEARTBEAT_DEFAULT_TIMEOUT_MS,
 *          {"service1", "service2", "service3"},
 *          [](const std::string& serviceName, bool alive) {
 *              // Handle service state changes here.
 *          });
 *
 *      monitor.Stop();
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __HEART_BEAT_MONITOR_H__
#define __HEART_BEAT_MONITOR_H__

#include <functional>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

#include "HeartbeatCommon.h"

class HeartbeatMonitor {
public:
    using Callback = std::function<void(const std::string&, bool)>;

    HeartbeatMonitor();
    ~HeartbeatMonitor();

    int32_t Start(const std::string& channel,
                  uint32_t timeoutMs,
                  const Callback& callback);
    int32_t Start(const std::string& channel,
                  uint32_t timeoutMs,
                  const std::vector<std::string>& services,
                  const Callback& callback);
    int32_t Reset(const std::string& serviceName);
    int32_t Stop();

private:
    HeartbeatMonitor(const HeartbeatMonitor&) = delete;
    HeartbeatMonitor& operator=(const HeartbeatMonitor&) = delete;

    int32_t StartInternal(const std::string& channel,
                          uint32_t timeoutMs,
                          const std::vector<std::string>& services,
                          const Callback& callback);
    void Run();
    void ProcessReports();
    void CheckTimeouts();
    void Notify(const std::string& serviceName, bool alive);

private:
    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

#endif // __HEART_BEAT_MONITOR_H__
