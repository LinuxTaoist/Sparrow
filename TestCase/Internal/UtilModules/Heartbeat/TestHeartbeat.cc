/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : TestHeartbeat.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Heartbeat module internal tests.
 *  @date       : 2026/09/17
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <vector>

#include "HeartbeatChannel.h"
#include "HeartbeatMonitor.h"
#include "HeartbeatReporter.h"
#include "gtest/gtest.h"

namespace {

std::string MakeChannel(const char* tag) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "/tmp/spr_heartbeat_%s_%d.sock", tag, static_cast<int>(getpid()));
    unlink(buffer);
    return buffer;
}

bool WaitForCount(const std::atomic<int>& count, int expected, uint32_t timeoutMs) {
    const uint32_t intervalMs = 10;
    for (uint32_t elapsedMs = 0; elapsedMs < timeoutMs; elapsedMs += intervalMs) {
        if (count.load() >= expected) {
            return true;
        }
        usleep(intervalMs * 1000);
    }
    return count.load() >= expected;
}

void SendInvalidMessage(const std::string& channel) {
    const int32_t fd = socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    ASSERT_GE(fd, 0);

    struct sockaddr_un address;
    std::memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    std::strncpy(address.sun_path, channel.c_str(), sizeof(address.sun_path) - 1);
    const char data[] = "invalid";
    EXPECT_EQ(static_cast<ssize_t>(sizeof(data) - 1),
              sendto(fd, data, sizeof(data) - 1, 0,
                     reinterpret_cast<const sockaddr*>(&address), sizeof(address)));
    close(fd);
}

void SendHeartbeat(const std::string& channel, const std::string& serviceName, uint64_t sequence) {
    HeartbeatChannel reporter;
    ASSERT_EQ(0, reporter.OpenReporter(channel));

    HeartbeatMessage message = {};
    message.magic = HEARTBEAT_PROTOCOL_MAGIC;
    message.version = HEARTBEAT_PROTOCOL_VERSION;
    message.nameLength = static_cast<uint16_t>(serviceName.size());
    message.sequence = sequence;
    std::memcpy(message.serviceName, serviceName.data(), message.nameLength);
    EXPECT_EQ(static_cast<ssize_t>(sizeof(message)), reporter.Send(message));
}

} // namespace

// 测试 Reporter 未启动、参数非法以及重复启动时的返回值
TEST(UtilModules_Heartbeat, ReporterParameterValidation) {
    HeartbeatReporter* reporter = HeartbeatReporter::GetInstance();
    ASSERT_NE(nullptr, reporter);
    const std::string channel = MakeChannel("reporter_validation");

    EXPECT_EQ(-1, reporter->Report());
    EXPECT_EQ(-1, reporter->Start("service_a", 50, ""));
    EXPECT_EQ(-1, reporter->Start("", 50, channel));
    EXPECT_EQ(-1, reporter->Start("service_a", 0, channel));
    EXPECT_EQ(-1, reporter->Start(std::string(HEARTBEAT_SERVICE_NAME_MAX_LENGTH + 1, 'a'), 50, channel));

    EXPECT_EQ(0, reporter->Start("service_a", 50, channel));
    EXPECT_EQ(-1, reporter->Start("service_b", 50, channel));
    reporter->Stop();
    reporter->Stop();
    unlink(channel.c_str());
}

// 测试 Monitor 参数校验、重复启动和重复停止
TEST(UtilModules_Heartbeat, MonitorParameterValidation) {
    HeartbeatMonitor monitor;
    const std::string channel = MakeChannel("monitor_validation");
    const std::vector<std::string> invalidServices = {
        "", std::string(HEARTBEAT_SERVICE_NAME_MAX_LENGTH + 1, 'a')};

    EXPECT_EQ(-1, monitor.Start("", 100, HeartbeatMonitor::Callback()));
    EXPECT_EQ(-1, monitor.Start(channel, 0, HeartbeatMonitor::Callback()));
    EXPECT_EQ(-1, monitor.Start(channel, 100, invalidServices, HeartbeatMonitor::Callback()));

    EXPECT_EQ(0, monitor.Start(channel, 100, HeartbeatMonitor::Callback()));
    EXPECT_EQ(-1, monitor.Start(channel, 100, HeartbeatMonitor::Callback()));
    monitor.Stop();
    monitor.Stop();
    unlink(channel.c_str());
}

// 测试单个服务的自动发现和首次心跳回调
TEST(UtilModules_Heartbeat, ReporterMonitorSingleService) {
    const std::string channel = MakeChannel("single");
    std::atomic<int> aliveCount(0);

    HeartbeatMonitor monitor;
    ASSERT_EQ(0, monitor.Start(channel, 300,
                               [&aliveCount](const std::string& serviceName, bool alive) {
                                   if (serviceName == "service_a" && alive) {
                                       ++aliveCount;
                                   }
                               }));

    HeartbeatReporter* reporter = HeartbeatReporter::GetInstance();
    ASSERT_NE(nullptr, reporter);
    ASSERT_EQ(0, reporter->Start("service_a", 50, channel));

    ASSERT_TRUE(WaitForCount(aliveCount, 1, 500));
    reporter->Stop();
    monitor.Stop();

    EXPECT_EQ(1, aliveCount.load());
    unlink(channel.c_str());
}

// 测试一个 Monitor 同时监听多个服务
TEST(UtilModules_Heartbeat, MonitorTracksMultipleServices) {
    const std::string channel = MakeChannel("multiple");
    std::atomic<int> serviceCount(0);

    HeartbeatMonitor monitor;
    const std::vector<std::string> services = {"service_a", "service_b"};
    ASSERT_EQ(0, monitor.Start(channel, 300, services,
                               [&serviceCount](const std::string& serviceName, bool alive) {
                                   if (alive && (serviceName == "service_a" || serviceName == "service_b")) {
                                       ++serviceCount;
                                   }
                               }));
    SendHeartbeat(channel, "service_a", 0);
    SendHeartbeat(channel, "service_b", 0);

    ASSERT_TRUE(WaitForCount(serviceCount, 2, 500));
    monitor.Stop();

    EXPECT_EQ(2, serviceCount.load());
    unlink(channel.c_str());
}

// 测试预期服务持续无心跳时按周期重复触发超时
TEST(UtilModules_Heartbeat, MonitorReportsExpectedServiceTimeout) {
    const std::string channel = MakeChannel("timeout");
    std::atomic<int> timeoutCount(0);
    std::vector<std::string> services;
    services.push_back("service_a");

    HeartbeatMonitor monitor;
    ASSERT_EQ(0, monitor.Start(channel, 100, services,
                               [&timeoutCount](const std::string& serviceName, bool alive) {
                                   if (serviceName == "service_a" && !alive) {
                                       ++timeoutCount;
                                   }
                               }));

    ASSERT_TRUE(WaitForCount(timeoutCount, 2, 350));
    monitor.Stop();

    EXPECT_EQ(2, timeoutCount.load());
    unlink(channel.c_str());
}

// 测试服务超时后的恢复回调
TEST(UtilModules_Heartbeat, MonitorReportsServiceRecovery) {
    const std::string channel = MakeChannel("recovery");
    std::atomic<int> aliveCount(0);
    std::atomic<int> timeoutCount(0);
    std::vector<std::string> services;
    services.push_back("service_a");

    HeartbeatMonitor monitor;
    ASSERT_EQ(0, monitor.Start(channel, 100, services,
                               [&aliveCount, &timeoutCount](const std::string& serviceName, bool alive) {
                                   if (serviceName != "service_a") {
                                       return;
                                   }
                                   if (alive) {
                                       ++aliveCount;
                                   } else {
                                       ++timeoutCount;
                                   }
                               }));

    HeartbeatReporter* reporter = HeartbeatReporter::GetInstance();
    ASSERT_NE(nullptr, reporter);
    ASSERT_EQ(0, reporter->Start("service_a", 30, channel));
    ASSERT_TRUE(WaitForCount(aliveCount, 1, 500));

    reporter->Stop();
    ASSERT_TRUE(WaitForCount(timeoutCount, 1, 500));

    ASSERT_EQ(0, reporter->Start("service_a", 30, channel));
    ASSERT_TRUE(WaitForCount(aliveCount, 2, 500));

    reporter->Stop();
    monitor.Stop();
    EXPECT_EQ(2, aliveCount.load());
    EXPECT_EQ(1, timeoutCount.load());
    unlink(channel.c_str());
}

// 测试 Reporter 停止后 Monitor 能够检测服务超时
TEST(UtilModules_Heartbeat, MonitorReportsReporterStop) {
    const std::string channel = MakeChannel("reporter_stop");
    std::atomic<int> timeoutCount(0);
    std::vector<std::string> services;
    services.push_back("service_a");

    HeartbeatMonitor monitor;
    ASSERT_EQ(0, monitor.Start(channel, 100, services,
                               [&timeoutCount](const std::string& serviceName, bool alive) {
                                   if (serviceName == "service_a" && !alive) {
                                       ++timeoutCount;
                                   }
                               }));

    HeartbeatReporter* reporter = HeartbeatReporter::GetInstance();
    ASSERT_NE(nullptr, reporter);
    ASSERT_EQ(0, reporter->Start("service_a", 30, channel));
    usleep(150000);
    reporter->Stop();
    ASSERT_TRUE(WaitForCount(timeoutCount, 1, 500));

    monitor.Stop();
    EXPECT_EQ(1, timeoutCount.load());
    unlink(channel.c_str());
}

// 测试手动 Report 能够在周期上报间隔之外立即发送心跳
TEST(UtilModules_Heartbeat, ReporterManualReport) {
    const std::string channel = MakeChannel("manual_report");
    std::atomic<int> aliveCount(0);

    HeartbeatMonitor monitor;
    ASSERT_EQ(0, monitor.Start(channel, 300,
                               [&aliveCount](const std::string& serviceName, bool alive) {
                                   if (serviceName == "service_a" && alive) {
                                       ++aliveCount;
                                   }
                               }));

    HeartbeatReporter* reporter = HeartbeatReporter::GetInstance();
    ASSERT_NE(nullptr, reporter);
    ASSERT_EQ(0, reporter->Start("service_a", 1000, channel));
    ASSERT_TRUE(WaitForCount(aliveCount, 1, 300));
    EXPECT_EQ(0, reporter->Report());

    reporter->Stop();
    monitor.Stop();
    EXPECT_EQ(1, aliveCount.load());
    unlink(channel.c_str());
}

// 测试非法心跳数据不会触发服务存活回调，也不会停止 Monitor
TEST(UtilModules_Heartbeat, MonitorIgnoresInvalidMessage) {
    const std::string channel = MakeChannel("invalid_message");
    std::atomic<int> callbackCount(0);

    HeartbeatMonitor monitor;
    ASSERT_EQ(0, monitor.Start(channel, 200,
                               [&callbackCount](const std::string&, bool) {
                                   ++callbackCount;
                               }));

    SendInvalidMessage(channel);
    usleep(150000);
    EXPECT_EQ(0, callbackCount.load());

    monitor.Stop();
    unlink(channel.c_str());
}

// 测试 Monitor Stop 能够唤醒监听线程并及时返回
TEST(UtilModules_Heartbeat, MonitorStopsPromptly) {
    const std::string channel = MakeChannel("stop");
    HeartbeatMonitor monitor;
    ASSERT_EQ(0, monitor.Start(channel, 1000, HeartbeatMonitor::Callback()));

    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    monitor.Stop();
    const uint64_t elapsedMs = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - begin).count());

    EXPECT_LT(elapsedMs, 500U);
    unlink(channel.c_str());
}
