/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : HeartbeatChannel.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Internal Unix domain datagram channel for Heartbeat.
 *  @date       : 2026/09/17
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "HeartbeatLog.h"
#include "HeartbeatChannel.h"

#define HBLOG_TAG "HBChannel"

HeartbeatChannel::HeartbeatChannel()
    : mFd(-1), mOwner(false) {
    std::memset(&mAddress, 0, sizeof(mAddress));
}

HeartbeatChannel::~HeartbeatChannel() {
    Close();
}

int32_t HeartbeatChannel::OpenReporter(const std::string& path) {
    if (path.empty() || path.size() >= sizeof(mAddress.sun_path)) {
        HBLOGE("Invalid path %s!\n", path.c_str());
        return -1;
    }

    Close();
    mFd = socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (mFd < 0) {
        HBLOGE("socket failed! (%s)\n", std::strerror(errno));
        return -1;
    }

    std::memset(&mAddress, 0, sizeof(mAddress));
    mAddress.sun_family = AF_UNIX;
    std::strncpy(mAddress.sun_path, path.c_str(), sizeof(mAddress.sun_path) - 1);
    mPath = path;
    mOwner = false;
    return 0;
}

int32_t HeartbeatChannel::OpenMonitor(const std::string& path) {
    if (path.empty() || path.size() >= sizeof(mAddress.sun_path)) {
        HBLOGE("Invalid path %s!\n", path.c_str());
        return -1;
    }

    Close();
    mFd = socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
    if (mFd < 0) {
        HBLOGE("socket failed! (%s)\n", std::strerror(errno));
        return -1;
    }

    std::memset(&mAddress, 0, sizeof(mAddress));
    mAddress.sun_family = AF_UNIX;
    std::strncpy(mAddress.sun_path, path.c_str(), sizeof(mAddress.sun_path) - 1);

    unlink(path.c_str());
    if (bind(mFd, reinterpret_cast<const sockaddr*>(&mAddress), sizeof(mAddress)) != 0) {
        HBLOGE("bind %s failed! (%s)\n", path.c_str(), std::strerror(errno));
        Close();
        return -1;
    }

    mPath = path;
    mOwner = true;
    return 0;
}

ssize_t HeartbeatChannel::Send(const HeartbeatMessage& message) {
    if (mFd < 0) {
        HBLOGE("Invalid fd!\n");
        return -1;
    }

    int32_t ret = sendto(mFd, &message, sizeof(message), 0,
                     reinterpret_cast<const sockaddr*>(&mAddress), sizeof(mAddress));
    if (ret < 0) {
        HBLOGE("sendto failed! (%s)\n", std::strerror(errno));
    }

    return ret;
}

ssize_t HeartbeatChannel::Receive(HeartbeatMessage& message) {
    if (mFd < 0) {
        HBLOGE("Invalid fd!\n");
        return -1;
    }

    return recvfrom(mFd, &message, sizeof(message), MSG_TRUNC, nullptr, nullptr);
}

int32_t HeartbeatChannel::GetFd() const {
    return mFd;
}

void HeartbeatChannel::Close() {
    if (mFd >= 0) {
        close(mFd);
        mFd = -1;
    }

    if (mOwner && !mPath.empty()) {
        unlink(mPath.c_str());
    }

    mOwner = false;
    mPath.clear();
}
