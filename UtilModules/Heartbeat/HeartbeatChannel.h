/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : HeartbeatChannel.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Internal Unix domain datagram channel for Heartbeat.
 *  @date       : 2026/09/17
 *
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __HEART_BEAT_CHANNEL_H__
#define __HEART_BEAT_CHANNEL_H__

#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <sys/un.h>

#include "HeartbeatCommon.h"

struct HeartbeatMessage
{
    uint32_t magic;
    uint16_t version;
    uint16_t nameLength;
    uint64_t sequence;
    char serviceName[HEARTBEAT_SERVICE_NAME_MAX_LENGTH];
};

class HeartbeatChannel
{
public:
    HeartbeatChannel();
    ~HeartbeatChannel();

    int32_t OpenReporter(const std::string& path);
    int32_t OpenMonitor(const std::string& path);
    ssize_t Send(const HeartbeatMessage& message);
    ssize_t Receive(HeartbeatMessage& message);
    int32_t GetFd() const;
    void Close();

private:
    HeartbeatChannel(const HeartbeatChannel&) = delete;
    HeartbeatChannel& operator=(const HeartbeatChannel&) = delete;

private:
    int32_t mFd;
    bool mOwner;
    std::string mPath;
    struct sockaddr_un mAddress;
};

#endif // __HEART_BEAT_CHANNEL_H__
