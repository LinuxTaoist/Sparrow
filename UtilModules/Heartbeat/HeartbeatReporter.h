/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : HeartbeatReporter.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Heartbeat reporter interface.
 *  @date       : 2026/09/17
 *
 *  Common usage:
 *      HeartbeatReporter* reporter = HeartbeatReporter::GetInstance();
 *      if (reporter != nullptr) {
 *          reporter->Start("my_service", 1000);
 *      }
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __HEART_BEAT_REPORTER_H__
#define __HEART_BEAT_REPORTER_H__

#include <memory>
#include <stdint.h>
#include <string>

#include "HeartbeatCommon.h"

class HeartbeatReporter {
public:
    static HeartbeatReporter* GetInstance();

    int32_t Start(const std::string& serviceName,
                  uint32_t intervalMs = HEARTBEAT_DEFAULT_INTERVAL_MS,
                  const std::string& channel = HEARTBEAT_DEFAULT_CHANNEL_PATH);
    int32_t Report();   // Manually report heartbeat.
    int32_t Stop();
    int32_t DumpDetails();

private:
    struct Impl;

    HeartbeatReporter();
    ~HeartbeatReporter();

    HeartbeatReporter(const HeartbeatReporter&) = delete;
    HeartbeatReporter& operator=(const HeartbeatReporter&) = delete;

    void Run();
    int32_t SendHeartbeat();

private:
    std::unique_ptr<Impl> mImpl;
};

#endif // __HEART_BEAT_REPORTER_H__
