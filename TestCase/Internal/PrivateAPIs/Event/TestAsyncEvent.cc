/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestAsyncEvent.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : AsyncEvent 异步事件通知内部测试
 *  @date       : 2026/09/10
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <ctime>
#include <mutex>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <condition_variable>
#include <poll.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include "Parcel.h"
#include "AsyncEvent.h"
#include "CoreTypeDefs.h"
#include "gtest/gtest.h"

using namespace InternalDefs;

namespace {
std::string MakeEventName() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    char buf[96] = {};
    std::snprintf(buf, sizeof(buf), "spr_aevent_%d_%ld", getpid(), ts.tv_nsec % 1000000L);
    return std::string(buf);
}

std::mutex gCallbackMutex;
std::condition_variable gCallbackCondition;
int gFirstCallbackCount = 0;
int gSecondCallbackCount = 0;
const int32_t TEST_EVENT_MAX_DATA_SIZE = 10 * 1024;
const int32_t PROCESS_EVENT_READY = -2;
int gProcessEventPipe = -1;

struct ProcessEventData {
    int32_t eventID;
    int32_t size;
    int32_t payload;
};

bool ReadEvent(Parcel& parcel, int timeout, int32_t& event, std::vector<uint8_t>& payload) {
    if (parcel.TimedWait(timeout) != 0) {
        return false;
    }

    std::vector<uint8_t> frame(TEST_EVENT_MAX_DATA_SIZE + sizeof(int32_t));
    int frameSize = 0;
    if (parcel.ReadData(frame.data(), frameSize, static_cast<int>(frame.size())) != 0 ||
        frameSize < static_cast<int>(sizeof(uint32_t))) {
        return false;
    }

    uint32_t networkEvent = 0;
    std::memcpy(&networkEvent, frame.data(), sizeof(networkEvent));
    event = static_cast<int32_t>(ntohl(networkEvent));
    payload.assign(frame.begin() + sizeof(networkEvent), frame.begin() + frameSize);
    return true;
}

bool ReadProcessEvent(int fd, ProcessEventData& result) {
    char* data = reinterpret_cast<char*>(&result);
    size_t remaining = sizeof(result);
    while (remaining > 0) {
        ssize_t size = read(fd, data, remaining);
        if (size < 0 && errno == EINTR) {
            continue;
        }
        if (size <= 0) {
            return false;
        }
        data += size;
        remaining -= static_cast<size_t>(size);
    }
    return true;
}

bool WriteProcessEvent(int fd, const ProcessEventData& result) {
    const char* data = reinterpret_cast<const char*>(&result);
    size_t remaining = sizeof(result);
    while (remaining > 0) {
        ssize_t size = write(fd, data, remaining);
        if (size < 0 && errno == EINTR) {
            continue;
        }
        if (size <= 0) {
            return false;
        }
        data += size;
        remaining -= static_cast<size_t>(size);
    }
    return true;
}

void FirstEventCallback(int32_t, void*, int32_t) {
    std::lock_guard<std::mutex> lock(gCallbackMutex);
    ++gFirstCallbackCount;
    gCallbackCondition.notify_all();
}

void SecondEventCallback(int32_t, void*, int32_t) {
    std::lock_guard<std::mutex> lock(gCallbackMutex);
    ++gSecondCallbackCount;
    gCallbackCondition.notify_all();
}

void ProcessEventCallback(int32_t eventID, void* data, int32_t size) {
    ProcessEventData result = {eventID, size, 0};
    if (data != nullptr && size >= static_cast<int32_t>(sizeof(result.payload))) {
        std::memcpy(&result.payload, data, sizeof(result.payload));
    }
    WriteProcessEvent(gProcessEventPipe, result);
}
}

class Util_AsyncEvent : public ::testing::Test {
protected:
    void SetUp() override {
        mName = MakeEventName();
    }

    void TearDown() override {
        AsyncEvent* event = AsyncEvent::GetInstance();
        event->DelWriter(mName);
        event->DelWriter(mName + "_first");
        event->DelWriter(mName + "_second");
        event->DelWriter(mName + "_slow");
        event->DelWriter(mName + "_fast");
        event->DelWriter(mName + "_service_pid_" + std::to_string(getpid()));
        event->DelWriter(mName + "_process_service_pid_" + std::to_string(getpid()));
        unlink(("/tmp/" + mName + "_callback_req").c_str());
        unlink(("/tmp/" + mName + "_callback_rsp").c_str());
        unlink(("/tmp/" + mName + "_process_callback_req").c_str());
        unlink(("/tmp/" + mName + "_process_callback_rsp").c_str());
        unlink(("/tmp/" + mName + "_event").c_str());
    }

    std::string mName;
};

// 测试单例模式返回非空
TEST_F(Util_AsyncEvent, SingletonReturnsSameInstance) {
    AsyncEvent* a = AsyncEvent::GetInstance();
    AsyncEvent* b = AsyncEvent::GetInstance();
    ASSERT_TRUE(a != nullptr);
    EXPECT_EQ(a, b);
}

// 测试没有 writer 时 EventNotify 返回失败
TEST_F(Util_AsyncEvent, EventNotifyWithoutWritersFails) {
    AsyncEvent* event = AsyncEvent::GetInstance();
    EXPECT_EQ(event->EventNotify(1, nullptr, 0), -1);
}

// 测试作为写者初始化并发送事件
TEST_F(Util_AsyncEvent, AddWriterAndNotify) {
    AsyncEvent* event = AsyncEvent::GetInstance();
    ASSERT_EQ(event->AddWriter(mName), 0);

    int32_t data = 12345;
    EXPECT_EQ(event->EventNotify(0x55, &data, sizeof(data)), 0);
    EXPECT_EQ(event->EventNotify(0x66, nullptr, 0), 0);
}

// 测试空回调和空 Binder Parcel 参数均被拒绝
TEST_F(Util_AsyncEvent, RegisterInvalidArgumentsFails) {
    AsyncEvent* event = AsyncEvent::GetInstance();
    EXPECT_EQ(event->RegisterEventCallback(mName, nullptr, nullptr, nullptr), -1);
    EXPECT_EQ(event->RegisterEventCallback(mName, nullptr, nullptr, FirstEventCallback), -1);
}

// 测试 Add/Del writer 拒绝空通道名
TEST_F(Util_AsyncEvent, WriterRejectsEmptyChannelName) {
    AsyncEvent* event = AsyncEvent::GetInstance();
    EXPECT_EQ(event->AddWriter(""), -1);
    EXPECT_EQ(event->DelWriter(""), -1);
}

// 测试重复添加同一 writer 不会重复广播，删除后不再接收事件
TEST_F(Util_AsyncEvent, AddWriterIsIdempotentAndDelWriterRemovesChannel) {
    AsyncEvent* event = AsyncEvent::GetInstance();
    ASSERT_EQ(event->AddWriter(mName), 0);
    ASSERT_EQ(event->AddWriter(mName), 0);

    Parcel reader(mName + "_event", 99999, false);
    ASSERT_EQ(event->EventNotify(0x31, nullptr, 0), 0);

    int32_t eventID = 0;
    std::vector<uint8_t> payload;
    ASSERT_TRUE(ReadEvent(reader, PARCEL_TIMEOUT_MS, eventID, payload));
    EXPECT_EQ(eventID, 0x31);
    EXPECT_TRUE(payload.empty());
    EXPECT_NE(reader.TimedWait(10), 0);

    EXPECT_EQ(event->DelWriter(mName), 0);
    EXPECT_EQ(event->DelWriter(mName), 0);
    EXPECT_EQ(event->EventNotify(0x32, nullptr, 0), -1);
}

// 测试 EventNotify 拒绝空指针、负长度和超出上限的数据
TEST_F(Util_AsyncEvent, EventNotifyRejectsInvalidPayload) {
    AsyncEvent* event = AsyncEvent::GetInstance();
    ASSERT_EQ(event->AddWriter(mName), 0);

    int32_t payload = 123;
    std::vector<uint8_t> oversizedPayload(TEST_EVENT_MAX_DATA_SIZE + 1, 0x5A);
    EXPECT_EQ(event->EventNotify(0x41, nullptr, sizeof(payload)), -1);
    EXPECT_EQ(event->EventNotify(0x42, &payload, -1), -1);
    EXPECT_EQ(event->EventNotify(0x43, oversizedPayload.data(),
                                 static_cast<int>(oversizedPayload.size())), -1);

    Parcel reader(mName + "_event", 99999, false);
    ASSERT_EQ(event->EventNotify(0x44, &payload, sizeof(payload)), 0);
    int32_t eventID = 0;
    std::vector<uint8_t> receivedPayload;
    ASSERT_TRUE(ReadEvent(reader, PARCEL_TIMEOUT_MS, eventID, receivedPayload));
    EXPECT_EQ(eventID, 0x44);
    ASSERT_EQ(receivedPayload.size(), sizeof(payload));
    EXPECT_EQ(std::memcmp(receivedPayload.data(), &payload, sizeof(payload)), 0);
}

// 测试广播事件可被所有独立订阅通道完整读取
TEST_F(Util_AsyncEvent, EventNotifyBroadcastsToAllReaders) {
    AsyncEvent* event = AsyncEvent::GetInstance();
    const std::string firstName = mName + "_first";
    const std::string secondName = mName + "_second";
    ASSERT_EQ(event->AddWriter(firstName), 0);
    ASSERT_EQ(event->AddWriter(secondName), 0);

    Parcel firstReader(firstName + "_event", 99999, false);
    Parcel secondReader(secondName + "_event", 99999, false);
    int32_t payload = 12345;
    ASSERT_EQ(event->EventNotify(0x55, &payload, sizeof(payload)), 0);

    int32_t firstEvent = 0;
    int32_t secondEvent = 0;
    std::vector<uint8_t> firstPayload;
    std::vector<uint8_t> secondPayload;
    ASSERT_TRUE(ReadEvent(firstReader, PARCEL_TIMEOUT_MS, firstEvent, firstPayload));
    ASSERT_TRUE(ReadEvent(secondReader, PARCEL_TIMEOUT_MS, secondEvent, secondPayload));

    EXPECT_EQ(firstEvent, 0x55);
    EXPECT_EQ(secondEvent, 0x55);
    ASSERT_EQ(firstPayload.size(), sizeof(payload));
    ASSERT_EQ(secondPayload.size(), sizeof(payload));
    EXPECT_EQ(std::memcmp(firstPayload.data(), &payload, sizeof(payload)), 0);
    EXPECT_EQ(std::memcmp(secondPayload.data(), &payload, sizeof(payload)), 0);
}

// 测试某个订阅者队列写满后不产生半帧，排空后可继续接收
TEST_F(Util_AsyncEvent, FullSubscriberQueueDoesNotCorruptNextEvent) {
    AsyncEvent* event = AsyncEvent::GetInstance();
    const std::string slowName = mName + "_slow";
    const std::string fastName = mName + "_fast";
    ASSERT_EQ(event->AddWriter(slowName), 0);
    ASSERT_EQ(event->AddWriter(fastName), 0);

    Parcel slowReader(slowName + "_event", 99999, false);
    Parcel fastReader(fastName + "_event", 99999, false);
    std::vector<uint8_t> payload(256, 0x5A);
    int acceptedCount = 0;
    bool queueFull = false;

    for (int32_t index = 0; index < 100; ++index) {
        int32_t eventID = 1000 + index;
        int ret = event->EventNotify(eventID, payload.data(), static_cast<int>(payload.size()));

        int32_t fastEvent = 0;
        std::vector<uint8_t> fastPayload;
        ASSERT_TRUE(ReadEvent(fastReader, PARCEL_TIMEOUT_MS, fastEvent, fastPayload));
        EXPECT_EQ(fastEvent, eventID);
        EXPECT_EQ(fastPayload, payload);

        if (ret != 0) {
            queueFull = true;
            break;
        }
        ++acceptedCount;
    }

    ASSERT_TRUE(queueFull);
    for (int index = 0; index < acceptedCount; ++index) {
        int32_t slowEvent = 0;
        std::vector<uint8_t> slowPayload;
        ASSERT_TRUE(ReadEvent(slowReader, PARCEL_TIMEOUT_MS, slowEvent, slowPayload));
        EXPECT_EQ(slowEvent, 1000 + index);
        EXPECT_EQ(slowPayload, payload);
    }
    EXPECT_NE(slowReader.TimedWait(10), 0);

    int32_t recoveredEvent = 0x77;
    ASSERT_EQ(event->EventNotify(recoveredEvent, payload.data(), static_cast<int>(payload.size())), 0);

    int32_t slowEvent = 0;
    int32_t fastEvent = 0;
    std::vector<uint8_t> slowPayload;
    std::vector<uint8_t> fastPayload;
    ASSERT_TRUE(ReadEvent(slowReader, PARCEL_TIMEOUT_MS, slowEvent, slowPayload));
    ASSERT_TRUE(ReadEvent(fastReader, PARCEL_TIMEOUT_MS, fastEvent, fastPayload));
    EXPECT_EQ(slowEvent, recoveredEvent);
    EXPECT_EQ(fastEvent, recoveredEvent);
    EXPECT_EQ(slowPayload, payload);
    EXPECT_EQ(fastPayload, payload);
}

// 测试同一进程中的多个回调均收到同一广播事件
TEST_F(Util_AsyncEvent, MultipleCallbacksReceiveSameEvent) {
    AsyncEvent* event = AsyncEvent::GetInstance();
    const std::string serviceName = mName + "_service";
    const std::string binderName = mName + "_callback";
    const int32_t binderKey = 99998;
    std::shared_ptr<Parcel> reqServer = std::make_shared<Parcel>(binderName + "_req", binderKey, true);
    std::shared_ptr<Parcel> rspServer = std::make_shared<Parcel>(binderName + "_rsp", binderKey, true);
    std::shared_ptr<Parcel> reqClient = std::make_shared<Parcel>(binderName + "_req", binderKey, false);
    std::shared_ptr<Parcel> rspClient = std::make_shared<Parcel>(binderName + "_rsp", binderKey, false);

    {
        std::lock_guard<std::mutex> lock(gCallbackMutex);
        gFirstCallbackCount = 0;
        gSecondCallbackCount = 0;
    }

    int serverRet = 0;
    std::thread binderThread([&]() {
        for (int i = 0; i < 4; ++i) {
            int32_t cmd = 0;
            std::string channelName;
            int32_t ret = reqServer->Wait();
            if (ret == 0) {
                ret = reqServer->ReadInt(cmd);
            }
            if (ret == 0) {
                ret = reqServer->ReadString(channelName);
            }
            if (ret == 0) {
                if (cmd == GENERAL_REGISTER_CALLBACK) {
                    ret = event->AddWriter(channelName);
                } else if (cmd == GENERAL_UNREGISTER_CALLBACK) {
                    ret = event->DelWriter(channelName);
                } else {
                    ret = -1;
                }
            }
            if (rspServer->WriteInt(ret) != 0 || rspServer->Post() != 0) {
                serverRet = -1;
                return;
            }
            if (ret != 0) {
                serverRet = ret;
            }
        }
    });

    int firstRet = event->RegisterEventCallback(
        serviceName, reqClient, rspClient, FirstEventCallback);
    int duplicateRet = event->RegisterEventCallback(
        serviceName, reqClient, rspClient, FirstEventCallback);
    int secondRet = event->RegisterEventCallback(
        serviceName, reqClient, rspClient, SecondEventCallback);
    EXPECT_EQ(firstRet, 0);
    EXPECT_EQ(duplicateRet, 0);
    EXPECT_EQ(secondRet, 0);

    if (firstRet == 0 && duplicateRet == 0 && secondRet == 0) {
        int32_t payload = 6789;
        EXPECT_EQ(event->EventNotify(0x66, &payload, sizeof(payload)), 0);

        std::unique_lock<std::mutex> lock(gCallbackMutex);
        bool received = gCallbackCondition.wait_for(lock, std::chrono::seconds(2), []() {
            return gFirstCallbackCount == 1 && gSecondCallbackCount == 1;
        });
        EXPECT_TRUE(received);
    }

    EXPECT_EQ(event->UnregisterEventCallback(serviceName, reqClient, rspClient), 0);
    binderThread.join();
    EXPECT_EQ(serverRet, 0);
    EXPECT_EQ(event->EventNotify(0x67, nullptr, 0), -1);
}

// 测试独立子进程注册后可收到广播并完成远端注销
TEST_F(Util_AsyncEvent, SeparateProcessReceivesBroadcastAndUnregisters) {
    AsyncEvent* event = AsyncEvent::GetInstance();
    const std::string serviceName = mName + "_process_service";
    const std::string binderName = mName + "_process_callback";
    const int32_t binderKey = 99997;
    std::shared_ptr<Parcel> reqServer = std::make_shared<Parcel>(binderName + "_req", binderKey, true);
    std::shared_ptr<Parcel> rspServer = std::make_shared<Parcel>(binderName + "_rsp", binderKey, true);

    int eventPipe[2] = {-1, -1};
    int controlPipe[2] = {-1, -1};
    ASSERT_EQ(pipe(eventPipe), 0);
    ASSERT_EQ(pipe(controlPipe), 0);

    pid_t child = fork();
    if (child == 0) {
        close(eventPipe[0]);
        close(controlPipe[1]);
        gProcessEventPipe = eventPipe[1];

        std::shared_ptr<Parcel> reqClient = std::make_shared<Parcel>(binderName + "_req", binderKey, false);
        std::shared_ptr<Parcel> rspClient = std::make_shared<Parcel>(binderName + "_rsp", binderKey, false);
        AsyncEvent* childEvent = AsyncEvent::GetInstance();
        int ret = childEvent ? childEvent->RegisterEventCallback(
            serviceName, reqClient, rspClient, ProcessEventCallback) : -1;
        if (ret != 0) {
            ProcessEventData result = {-1, ret, 0};
            WriteProcessEvent(gProcessEventPipe, result);
            _exit(1);
        }

        ProcessEventData ready = {PROCESS_EVENT_READY, 0, 0};
        if (!WriteProcessEvent(gProcessEventPipe, ready)) {
            _exit(4);
        }

        char ack = 0;
        if (read(controlPipe[0], &ack, sizeof(ack)) != static_cast<ssize_t>(sizeof(ack))) {
            _exit(2);
        }
        ret = childEvent->UnregisterEventCallback(serviceName, reqClient, rspClient);
        _exit(ret == 0 ? 0 : 3);
    }

    close(eventPipe[1]);
    close(controlPipe[0]);
    if (child < 0) {
        close(eventPipe[0]);
        close(controlPipe[1]);
        FAIL() << "fork failed";
    }

    auto handleBinderRequest = [&](int32_t expectedCommand, std::string& channelName) -> int32_t {
        int32_t ret = reqServer->TimedWait(PARCEL_TIMEOUT_MS);
        if (ret != 0) {
            return ret;
        }

        int32_t command = 0;
        ret = reqServer->ReadInt(command);
        if (ret == 0) {
            ret = reqServer->ReadString(channelName);
        }

        int32_t response = ret;
        if (ret == 0 && command == GENERAL_REGISTER_CALLBACK) {
            response = event->AddWriter(channelName);
        } else if (ret == 0 && command == GENERAL_UNREGISTER_CALLBACK) {
            response = event->DelWriter(channelName);
        } else if (ret == 0) {
            response = -1;
        }

        if (rspServer->WriteInt(response) != 0 || rspServer->Post() != 0) {
            return -1;
        }
        return (command == expectedCommand) ? response : -1;
    };

    std::string channelName;
    bool passed = (handleBinderRequest(GENERAL_REGISTER_CALLBACK, channelName) == 0);
    ProcessEventData received = {};
    if (passed) {
        struct pollfd descriptor = {eventPipe[0], POLLIN, 0};
        passed = (poll(&descriptor, 1, 3000) > 0 &&
                  ReadProcessEvent(eventPipe[0], received) &&
                  received.eventID == PROCESS_EVENT_READY);
    }
    if (passed) {
        int32_t payload = 6789;
        passed = (event->EventNotify(0x71, &payload, sizeof(payload)) == 0);
    }
    if (passed) {
        struct pollfd descriptor = {eventPipe[0], POLLIN, 0};
        passed = (poll(&descriptor, 1, 3000) > 0 &&
                  ReadProcessEvent(eventPipe[0], received));
    }
    if (passed) {
        passed = received.eventID == 0x71 && received.size == sizeof(received.payload) &&
                 received.payload == 6789;
    }

    if (passed) {
        char ack = 1;
        passed = (write(controlPipe[1], &ack, sizeof(ack)) == static_cast<ssize_t>(sizeof(ack)));
    }
    if (passed) {
        std::string removedChannel;
        passed = (handleBinderRequest(GENERAL_UNREGISTER_CALLBACK, removedChannel) == 0 &&
                  removedChannel == channelName);
    }
    if (passed) {
        passed = (event->EventNotify(0x72, nullptr, 0) == -1);
    }

    if (!passed) {
        kill(child, SIGKILL);
    }
    close(eventPipe[0]);
    close(controlPipe[1]);

    int status = 0;
    ASSERT_EQ(waitpid(child, &status, 0), child);
    if (!channelName.empty()) {
        event->DelWriter(channelName);
    }
    EXPECT_TRUE(passed);
    ASSERT_TRUE(WIFEXITED(status));
    EXPECT_EQ(WEXITSTATUS(status), 0);
}
