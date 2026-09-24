/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestBenchmark_MQueueThread.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SprObserverWithMQueueThread benchmark cases
 *  @date       : 2026/09/11
 *---------------------------------------------------------------------------------------------------------------------
 */

#include <atomic>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <functional>
#include <vector>
#include <algorithm>
#include <numeric>
#include "gtest/gtest.h"
#include "SprMsg.h"
#include "BenchCommon.h"
#include "SprEpollSchedule.h"
#include "SprObserverWithMQueueThread.h"

using namespace InternalDefs;

#define SPARROW_BENCH_MQTHREAD_ID_BASE_START 320

namespace {
constexpr int32_t MQTHREAD_DRAIN_TIMEOUT_MS = 5000;

class BenchObserverThread : public SprObserverWithMQueueThread {
public:
    explicit BenchObserverThread(uint16_t id)
        : SprObserverWithMQueueThread((ModuleIDType)(MODULE_PUBLIC_END + id), "bench_th", MEDIATOR_PROXY_MQUEUE) {
    }

    ~BenchObserverThread() {
        Deinitialize();
    }

    int32_t GetRecvCount() const {
        return mRecvCount.load();
    }

    SprMsg GetMsg() const {
        std::lock_guard<std::mutex> lock(mMsgMutex);
        return mMsg;
    }

    std::vector<uint64_t> GetLatencySamples() const {
        std::lock_guard<std::mutex> lock(mMsgMutex);
        return mLatencyUs;
    }

private:
    int32_t Init() override {
        return 0;
    }

    int32_t ProcessMsg(const SprMsg& msg) override {
        if (msg.GetMsgId() != SIG_ID_TEST_BENCHMARK_MQ_EVENT) {
            return 0;
        }
        uint64_t sendUs = msg.GetU64Value();
        uint64_t nowUs = BenchNowUs();
        uint64_t latencyUs = (sendUs != 0 && nowUs >= sendUs) ? (nowUs - sendUs) : 0ULL;
        {
            std::lock_guard<std::mutex> lock(mMsgMutex);
            mMsg = msg;
            mLatencyUs.push_back(latencyUs);
        }
        mRecvCount.fetch_add(1);
        return 0;
    }

private:
    mutable std::mutex mMsgMutex;
    SprMsg mMsg;
    std::vector<uint64_t> mLatencyUs;
    std::atomic<int32_t> mRecvCount { 0 };
};
} // namespace

class Core_SprObsMQThreadBench : public ::testing::Test {
protected:
    Core_SprObsMQThreadBench()
        : mSender(SPARROW_BENCH_MQTHREAD_ID_BASE_START),
          mReceiver((uint16_t)(SPARROW_BENCH_MQTHREAD_ID_BASE_START + 1)) {}

    static void SetUpTestSuite() {
        BenchInstallReportListener();
        BenchInstallGlobalEnvironment();

        sEpollThread = std::thread([]() {
            SprEpollSchedule::GetInstance(0, 2000)->EpollLoop();
        });
    }

    static void TearDownTestSuite() {
        SprEpollSchedule::GetInstance(0, 2000)->ExitLoop();
        if (sEpollThread.joinable()) {
            sEpollThread.join();
        }

        BenchPrintModuleSummary("MQueueThread");
    }

    void SetUp() override {
        mSender.Initialize();
        mReceiver.Initialize();
    }

    void TearDown() override {
    }

protected:
    static std::thread sEpollThread;
    BenchObserverThread mSender;
    BenchObserverThread mReceiver;

    void PrepareCaseState() {
        const int32_t timeoutMs = MQTHREAD_DRAIN_TIMEOUT_MS;
        const int32_t stepMs = 5;
        const int32_t quietMs = 50;

        int32_t lastRecv = mReceiver.GetRecvCount();
        uint32_t lastPending = mReceiver.GetPendingMsgCount();
        int32_t stableMs = 0;
        int32_t elapsedMs = 0;
        while (elapsedMs < timeoutMs) {
            std::this_thread::sleep_for(std::chrono::milliseconds(stepMs));
            elapsedMs += stepMs;

            int32_t curRecv = mReceiver.GetRecvCount();
            uint32_t curPending = mReceiver.GetPendingMsgCount();
            if (curRecv == lastRecv && curPending == lastPending && curPending == 0U) {
                stableMs += stepMs;
                if (stableMs >= quietMs) {
                    return;
                }
            } else {
                lastRecv = curRecv;
                lastPending = curPending;
                stableMs = 0;
            }
        }
    }

    int32_t SendAToB(SprMsg& msg) {
        return mSender.NotifyObserver(mReceiver.GetModuleId(), msg);
    }

    void RunDualThreadLoad1sScenario(const char* scenario, int32_t targetOps) {
        PrepareCaseState();
        int32_t recvBase = mReceiver.GetRecvCount();
        uint32_t pendingBase = mReceiver.GetPendingMsgCount();
        uint64_t t0 = BenchNowUs();

        std::atomic<int32_t> sendOk(0);
        std::atomic<int32_t> sendFail(0);
        std::thread senderThread([&]() {
            SprMsg msg(SIG_ID_TEST_BENCHMARK_MQ_EVENT);
            msg.SetString("dual thread load");
            const auto start = std::chrono::steady_clock::now();
            for (int32_t i = 0; i < targetOps; ++i) {
                auto due = start + std::chrono::microseconds((int64_t)(i + 1) * 1000000LL / (int64_t)targetOps);
                std::this_thread::sleep_until(due);

                msg.SetU32Value((uint32_t)i);
                msg.SetU64Value((uint64_t)BenchNowUs());
                msg.SetString("dual thread load #" + std::to_string(i));
                if (SendAToB(msg) == 0) {
                    sendOk.fetch_add(1);
                } else {
                    sendFail.fetch_add(1);
                }
            }
        });
        senderThread.join();

        bool drained = BenchWaitUntil([&]() {
            int32_t processedNow = mReceiver.GetRecvCount();
            uint32_t pendingNow = mReceiver.GetPendingMsgCount();
            int32_t totalNow = processedNow + (int32_t)pendingNow;
            int32_t totalBase = recvBase + (int32_t)pendingBase;
            return totalNow >= (totalBase + sendOk.load());
        }, MQTHREAD_DRAIN_TIMEOUT_MS);

        std::vector<uint64_t> latenciesUs = mReceiver.GetLatencySamples();
        std::vector<uint64_t> validLatencies;
        validLatencies.reserve(latenciesUs.size());
        for (size_t i = 0; i < latenciesUs.size(); ++i) {
            if (latenciesUs[i] > 0ULL) {
                validLatencies.push_back(latenciesUs[i]);
            }
        }

        int32_t processedDelta = mReceiver.GetRecvCount() - recvBase;
        if (processedDelta < 0) {
            processedDelta = 0;
        }

        uint32_t pendingNow = mReceiver.GetPendingMsgCount();
        int32_t pendingDelta = (pendingNow >= pendingBase) ? (int32_t)(pendingNow - pendingBase) : 0;

        int32_t accepted = processedDelta + pendingDelta;
        int32_t sendOkValue = sendOk.load();
        if (accepted > sendOkValue) {
            accepted = sendOkValue;
        }

        int32_t sendFailValue = sendFail.load();
        int32_t recvOk = (int32_t)validLatencies.size();
        if (recvOk > accepted) {
            recvOk = accepted;
        }
        int32_t dataOk = recvOk;
        int32_t recvFail = sendOkValue - recvOk;
        int32_t dataFail = recvFail;

        uint64_t elapsedUs = BenchNowUs() - t0;
        double lossRate = (double)(targetOps - dataOk) / (double)targetOps;

        if (!validLatencies.empty()) {
            BenchRecordByTimes("MQueueThread", scenario, validLatencies, (uint64_t)recvOk, lossRate,
                                "attempt=" + std::to_string(targetOps)
                                + ",send_ok=" + std::to_string(sendOkValue)
                                + ",recv_ok=" + std::to_string(recvOk)
                                + ",data_ok=" + std::to_string(dataOk)
                                + ",processed_ok=" + std::to_string(processedDelta)
                                + ",pending_base=" + std::to_string(pendingBase)
                                + ",pending_now=" + std::to_string(pendingNow)
                                + ",pending_delta=" + std::to_string(pendingDelta)
                                + ",send_fail=" + std::to_string(sendFailValue)
                                + ",recv_fail=" + std::to_string(recvFail)
                                + ",data_fail=" + std::to_string(dataFail)
                                + ",drained=" + std::to_string(drained ? 1 : 0),
                                elapsedUs > 0 ? (double)dataOk * 1000000.0 / (double)elapsedUs : 0.0);
        } else {
            BenchResult r = {0, 0, 0, 0};
            BenchRecordBySummary("MQueueThread", scenario, r,
                                elapsedUs > 0 ? (double)dataOk * 1000000.0 / (double)elapsedUs : 0.0,
                                lossRate,
                                "attempt=" + std::to_string(targetOps)
                                + ",send_ok=" + std::to_string(sendOkValue)
                                + ",recv_ok=" + std::to_string(recvOk)
                                + ",data_ok=" + std::to_string(dataOk)
                                + ",processed_ok=" + std::to_string(processedDelta)
                                + ",pending_base=" + std::to_string(pendingBase)
                                + ",pending_now=" + std::to_string(pendingNow)
                                + ",pending_delta=" + std::to_string(pendingDelta)
                                + ",send_fail=" + std::to_string(sendFailValue)
                                + ",recv_fail=" + std::to_string(recvFail)
                                + ",data_fail=" + std::to_string(dataFail)
                                + ",drained=" + std::to_string(drained ? 1 : 0));
        }

        BenchLog("SprObsMQThread dual thread %s: attempt=%d send_ok=%d recv_ok=%d data_ok=%d send_fail=%d recv_fail=%d data_fail=%d processed=%d pending_delta=%d drained=%d loss=%.2f%% avg=%.1f us/op rate=%.1f msg/s",
                 scenario, targetOps, sendOkValue, recvOk, dataOk, sendFailValue, recvFail, dataFail,
                 processedDelta, pendingDelta, drained ? 1 : 0, lossRate * 100.0,
                 !validLatencies.empty() ? (double)std::accumulate(validLatencies.begin(), validLatencies.end(), 0ULL) / (double)validLatencies.size() : 0.0,
                 elapsedUs > 0 ? (double)dataOk * 1000000.0 / (double)elapsedUs : 0.0);
    }
};

std::thread Core_SprObsMQThreadBench::sEpollThread;

// 测试 1 秒内发送 200 条消息的吞吐和丢包情况
TEST_F(Core_SprObsMQThreadBench, Load1s200OpsDualThread) {
    RunDualThreadLoad1sScenario("Load1s200Ops", 200);
}

// 测试 1 秒内发送 500 条消息的吞吐和丢包情况
TEST_F(Core_SprObsMQThreadBench, Load1s500OpsDualThread) {
    RunDualThreadLoad1sScenario("Load1s500Ops", 500);
}

// 测试 1 秒内发送 1000 条消息的吞吐和丢包情况
TEST_F(Core_SprObsMQThreadBench, Load1s1000OpsDualThread) {
    RunDualThreadLoad1sScenario("Load1s1000Ops", 1000);
}

// 测试 1 秒内发送 10000 条消息的吞吐和丢包情况
TEST_F(Core_SprObsMQThreadBench, Load1s10000OpsDualThread) {
    RunDualThreadLoad1sScenario("Load1s10000Ops", 10000);
}
