/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : HeartbeatMonitor.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Heartbeat monitor implementation.
 *  @date       : 2026/09/17
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <map>
#include <mutex>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <poll.h>
#include <thread>
#include <sys/eventfd.h>
#include <unistd.h>
#include "HeartbeatChannel.h"
#include "HeartbeatLog.h"
#include "HeartbeatMonitor.h"

namespace {

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

#define HBLOG_TAG "HBMonitor"

struct ServiceStatus {
    bool alive;
    bool received;
    uint64_t sequence;
    TimePoint lastReport;

    ServiceStatus()
        : alive(false), received(false), sequence(0), lastReport(Clock::now()) {
    }
};

} // namespace

struct HeartbeatMonitor::Impl {
    HeartbeatChannel channel;
    uint32_t timeoutMs;
    Callback callback;
    std::map<std::string, ServiceStatus> services;
    bool running;
    int32_t wakeFd;
    std::mutex mutex;
    std::thread thread;

    Impl()
        : timeoutMs(HEARTBEAT_DEFAULT_TIMEOUT_MS), running(false), wakeFd(-1) {
    }
};

HeartbeatMonitor::HeartbeatMonitor()
    : mImpl(new Impl()) {
}

HeartbeatMonitor::~HeartbeatMonitor() {
    Stop();
}

int32_t HeartbeatMonitor::Start(const std::string& channel,
                                uint32_t timeoutMs,
                                const Callback& callback) {
    return StartInternal(channel, timeoutMs, std::vector<std::string>(), callback);
}

int32_t HeartbeatMonitor::Start(const std::string& channel,
                                uint32_t timeoutMs,
                                const std::vector<std::string>& services,
                                const Callback& callback) {
    return StartInternal(channel, timeoutMs, services, callback);
}

int32_t HeartbeatMonitor::StartInternal(const std::string& channel,
                                        uint32_t timeoutMs,
                                        const std::vector<std::string>& services,
                                        const Callback& callback) {
    if (channel.empty() || timeoutMs == 0) {
        HBLOGE("Invalid params!\n");
        return -1;
    }

    for (const std::string& service : services) {
        if (service.empty() || service.size() > HEARTBEAT_SERVICE_NAME_MAX_LENGTH) {
            HBLOGE("Invalid service name %s!\n", service.c_str());
            return -1;
        }
    }

    std::lock_guard<std::mutex> lock(mImpl->mutex);
    if (mImpl->running) {
        HBLOGE("Monitor already running!\n");
        return -1;
    }

    if (mImpl->channel.OpenMonitor(channel) != 0) {
        HBLOGE("Open monitor channel failed!\n");
        return -1;
    }

    mImpl->wakeFd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (mImpl->wakeFd < 0) {
        HBLOGE("eventfd failed! (%s)\n", std::strerror(errno));
        mImpl->channel.Close();
        return -1;
    }

    mImpl->timeoutMs = timeoutMs;
    mImpl->callback = callback;
    mImpl->services.clear();
    for (const std::string& service : services) {
        mImpl->services.emplace(service, ServiceStatus());
    }
    mImpl->running = true;
    mImpl->thread = std::thread(&HeartbeatMonitor::Run, this);
    return 0;
}

int32_t HeartbeatMonitor::Stop() {
    int32_t ret = 0;
    {
        std::lock_guard<std::mutex> lock(mImpl->mutex);
        if (!mImpl->running) {
            return ret;
        }
        mImpl->running = false;
    }

    uint64_t value = 1;
    if (mImpl->wakeFd >= 0) {
        ret = write(mImpl->wakeFd, &value, sizeof(value));
    }
    if (mImpl->thread.joinable()) {
        mImpl->thread.join();
    }
    mImpl->channel.Close();
    if (mImpl->wakeFd >= 0) {
        close(mImpl->wakeFd);
        mImpl->wakeFd = -1;
    }
    mImpl->services.clear();
    mImpl->callback = Callback();
    return ret;
}

int32_t HeartbeatMonitor::Reset(const std::string& serviceName) {
    if (serviceName.empty()) {
        return -1;
    }

    std::lock_guard<std::mutex> lock(mImpl->mutex);
    std::map<std::string, ServiceStatus>::iterator it = mImpl->services.find(serviceName);
    if (it == mImpl->services.end()) {
        return -1;
    }

    it->second.alive = false;
    it->second.received = false;
    it->second.lastReport = Clock::now();
    return 0;
}

void HeartbeatMonitor::Run() {
    while (true) {
        {
            std::lock_guard<std::mutex> lock(mImpl->mutex);
            if (!mImpl->running) {
                break;
            }
        }

        struct pollfd pollFds[2];
        pollFds[0].fd = mImpl->channel.GetFd();
        pollFds[0].events = POLLIN;
        pollFds[0].revents = 0;
        pollFds[1].fd = mImpl->wakeFd;
        pollFds[1].events = POLLIN;
        pollFds[1].revents = 0;

        int32_t result = poll(pollFds, 2, 100);
        if (result > 0 && (pollFds[1].revents & POLLIN) != 0) {
            uint64_t value = 0;
            int32_t ret = read(mImpl->wakeFd, &value, sizeof(value));
            if (ret < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                HBLOGE("read %d failed! (%s)\n", mImpl->wakeFd, std::strerror(errno));
            }
            continue;
        }
        if (result > 0 && (pollFds[0].revents & POLLIN) != 0) {
            ProcessReports();
        } else if (result < 0 && errno != EINTR) {
            HBLOGE("poll failed! (%s)\n", std::strerror(errno));
            break;
        }

        CheckTimeouts();
    }
}

void HeartbeatMonitor::ProcessReports() {
    while (true) {
        HeartbeatMessage message;
        std::memset(&message, 0, sizeof(message));
        ssize_t size = mImpl->channel.Receive(message);
        if (size <= 0) {
            break;
        }

        if (size != static_cast<ssize_t>(sizeof(message)) ||
            message.magic != HEARTBEAT_PROTOCOL_MAGIC ||
            message.version != HEARTBEAT_PROTOCOL_VERSION ||
            message.nameLength == 0 ||
            message.nameLength > HEARTBEAT_SERVICE_NAME_MAX_LENGTH) {
            continue;
        }

        std::string serviceName(message.serviceName, message.nameLength);
        std::lock_guard<std::mutex> lock(mImpl->mutex);
        ServiceStatus& status = mImpl->services[serviceName];
        bool notify = !status.alive;
        status.alive = true;
        status.received = true;
        status.sequence = message.sequence;
        status.lastReport = Clock::now();
        if (notify) {
            Notify(serviceName, true);
        }
    }
}

void HeartbeatMonitor::CheckTimeouts() {
    std::lock_guard<std::mutex> lock(mImpl->mutex);
    const TimePoint now = Clock::now();
    const std::chrono::milliseconds timeout(mImpl->timeoutMs);

    for (std::map<std::string, ServiceStatus>::iterator it = mImpl->services.begin();
         it != mImpl->services.end(); ++it) {
        ServiceStatus& status = it->second;
        if (now - status.lastReport < timeout) {
            continue;
        }

        // Report every timeout interval while no new heartbeat is received.
        status.alive = false;
        Notify(it->first, false);
        status.lastReport = now;
    }
}

void HeartbeatMonitor::Notify(const std::string& serviceName, bool alive) {
    if (mImpl->callback) {
        mImpl->callback(serviceName, alive);
    }
}
