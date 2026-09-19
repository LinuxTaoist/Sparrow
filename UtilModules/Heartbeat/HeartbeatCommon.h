/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : HeartbeatCommon.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Common definitions for the Heartbeat module.
 *  @date       : 2026/09/17
 *
 *  The reporter and monitor use HEARTBEAT_DEFAULT_CHANNEL_PATH by default.
 *  Include this header when a service needs to share the channel path or
 *  default timing values with the Heartbeat module.
 *
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __HEART_BEAT_COMMON_H__
#define __HEART_BEAT_COMMON_H__

#include <stdint.h>
#include <string>

#define HEARTBEAT_DEFAULT_CHANNEL_PATH       "/tmp/heartbeat.sock"
#define HEARTBEAT_DEFAULT_INTERVAL_MS        1000U
#define HEARTBEAT_DEFAULT_TIMEOUT_MS         10000U
#define HEARTBEAT_SERVICE_NAME_MAX_LENGTH    64U
#define HEARTBEAT_PACKET_MAX_LENGTH          256U
#define HEARTBEAT_PROTOCOL_MAGIC             0x48525442U
#define HEARTBEAT_PROTOCOL_VERSION           1U

enum HeartbeatState
{
    HEARTBEAT_STATE_UNKNOWN = 0,
    HEARTBEAT_STATE_ALIVE,
    HEARTBEAT_STATE_TIMEOUT
};

struct HeartbeatInfo
{
    std::string serviceName;
    HeartbeatState state;
    uint64_t sequence;
    uint64_t lastReportTimeMs;

    HeartbeatInfo()
        : state(HEARTBEAT_STATE_UNKNOWN), sequence(0), lastReportTimeMs(0)
    {
    }
};

#endif // __HEART_BEAT_COMMON_H__
