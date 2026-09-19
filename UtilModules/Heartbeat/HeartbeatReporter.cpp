/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : HeartbeatReporter.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Heartbeat reporter implementation.
 *  @date       : 2026/09/17
 *
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <thread>
#include "HeartbeatLog.h"
#include "HeartbeatChannel.h"
#include "HeartbeatReporter.h"

#define HBLOG_TAG "HBReporter"

static std::atomic<bool> gObjAlive(true);

struct HeartbeatReporter::Impl {
    HeartbeatChannel channel;
    std::string serviceName;
    std::string channelPath;
    uint32_t intervalMs;
    uint64_t sequence;
    std::atomic<bool> running;
    std::mutex mutex;
    std::mutex sendMutex;
    std::condition_variable condition;
    std::thread thread;

    Impl()
        : intervalMs(HEARTBEAT_DEFAULT_INTERVAL_MS), sequence(0), running(false) {
    }
};

HeartbeatReporter* HeartbeatReporter::GetInstance() {
    if (!gObjAlive) {
        return nullptr;
    }

    static HeartbeatReporter reporter;
    return &reporter;
}

HeartbeatReporter::HeartbeatReporter()
    : mImpl(new Impl()) {
}

HeartbeatReporter::~HeartbeatReporter() {
    Stop();
    gObjAlive = false;
}

int32_t HeartbeatReporter::Start(const std::string& serviceName,
                                 uint32_t intervalMs,
                                 const std::string& channel) {
    if (channel.empty() || serviceName.empty() || intervalMs == 0 ||
        serviceName.size() > HEARTBEAT_SERVICE_NAME_MAX_LENGTH) {
        HBLOGE("Invalid reporter arguments!\n");
        return -1;
    }

    std::lock_guard<std::mutex> lock(mImpl->mutex);
    if (mImpl->running.load()) {
        HBLOGE("Reporter already running!\n");
        return -1;
    }

    if (mImpl->channel.OpenReporter(channel) != 0) {
        HBLOGE("Open reporter channel failed!\n");
        return -1;
    }

    mImpl->serviceName = serviceName;
    mImpl->channelPath = channel;
    mImpl->intervalMs = intervalMs;
    mImpl->sequence = 0;
    mImpl->running = true;
    mImpl->thread = std::thread(&HeartbeatReporter::Run, this);
    return 0;
}

int32_t HeartbeatReporter::Report() {
    std::lock_guard<std::mutex> lock(mImpl->sendMutex);
    if (!mImpl->running.load()) {
        HBLOGE("Reporter is not running!\n");
        return -1;
    }

    return SendHeartbeat();
}

int32_t HeartbeatReporter::Stop() {
    {
        std::lock_guard<std::mutex> lock(mImpl->mutex);
        if (!mImpl->running.load()) {
            return -1;
        }
        mImpl->running = false;
    }

    mImpl->condition.notify_all();
    if (mImpl->thread.joinable()) {
        mImpl->thread.join();
    }
    std::lock_guard<std::mutex> sendLock(mImpl->sendMutex);
    mImpl->channel.Close();
    return 0;
}

int32_t HeartbeatReporter::DumpDetails() {
    std::string serviceName;
    std::string channelPath;
    uint32_t intervalMs = 0;
    uint64_t sequence = 0;
    bool running = false;
    int32_t channelFd = -1;

    // Keep the snapshot consistent with Start/Stop and SendHeartbeat.
    std::lock_guard<std::mutex> stateLock(mImpl->mutex);
    std::lock_guard<std::mutex> sendLock(mImpl->sendMutex);
    serviceName = mImpl->serviceName;
    channelPath = mImpl->channelPath;
    intervalMs = mImpl->intervalMs;
    sequence = mImpl->sequence;
    running = mImpl->running.load();
    channelFd = mImpl->channel.GetFd();

    HBLOGI("==============================================================================\n");
    HBLOGI("                         Heartbeat Reporter                                   \n");
    HBLOGI("==============================================================================\n");
    HBLOGI("\n");
    HBLOGI("  State     : %s\n",    running ? "Running" : "Stopped");
    HBLOGI("  Service   : %s\n",    serviceName.empty() ? "<none>" : serviceName.c_str());
    HBLOGI("  Interval  : %u ms\n", intervalMs);
    HBLOGI("  Channel   : %s\n",    channelPath.empty() ? "<none>" : channelPath.c_str());
    HBLOGI("  ChannelFd : %d\n",    channelFd);
    HBLOGI("  Sequence  : %llu\n",  static_cast<unsigned long long>(sequence));
    HBLOGI("\n");
    HBLOGI("==============================================================================\n");
    return 0;
}

void HeartbeatReporter::Run() {
    {
        std::lock_guard<std::mutex> lock(mImpl->sendMutex);
        if (mImpl->running.load()) {
            SendHeartbeat();
        }
    }

    while (true) {
        {
            std::unique_lock<std::mutex> lock(mImpl->mutex);
            if (mImpl->condition.wait_for(lock, std::chrono::milliseconds(mImpl->intervalMs),
                                          [this]() { return !mImpl->running.load(); })) {
                break;
            }
        }

        std::lock_guard<std::mutex> lock(mImpl->sendMutex);
        if (!mImpl->running.load()) {
            break;
        }
        SendHeartbeat();
    }
}

int32_t HeartbeatReporter::SendHeartbeat() {
    HeartbeatMessage message;
    std::memset(&message, 0, sizeof(message));
    message.magic = HEARTBEAT_PROTOCOL_MAGIC;
    message.version = HEARTBEAT_PROTOCOL_VERSION;
    message.nameLength = static_cast<uint16_t>(mImpl->serviceName.size());
    message.sequence = mImpl->sequence;
    std::memcpy(message.serviceName, mImpl->serviceName.data(), message.nameLength);

    int32_t ret = mImpl->channel.Send(message);
    if (ret < 0) {
        HBLOGE("Send heartbeat failed!\n");
        return -1;
    }

    ++mImpl->sequence;
    return 0;
}
