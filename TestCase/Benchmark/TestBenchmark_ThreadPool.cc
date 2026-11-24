/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestBenchmark_ThreadPool.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SprThreadPool benchmark cases
 *  @date       : 2026/09/11
 *---------------------------------------------------------------------------------------------------------------------
 */

#include <atomic>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <numeric>
#include "gtest/gtest.h"
#include "BenchCommon.h"
#include "SprThreadPool.h"

namespace {
constexpr int32_t THREADPOOL_DRAIN_TIMEOUT_MS = 5000;
}

class Core_ThreadPoolBench : public ::testing::TestWithParam<int32_t> {
protected:
    static void SetUpTestSuite() {
        BenchInstallReportListener();
        BenchInstallGlobalEnvironment();
    }

    static void TearDownTestSuite() {
        BenchPrintModuleSummary("ThreadPool");
    }

    void SetUp() override {
        mPool = SprThreadPool::GetInstance();
    }

    void TearDown() override {
        mPool = nullptr;
    }

    void RunLoad1sScenario(const char* scenario, int32_t targetOps) {
        std::atomic<int32_t> done(0);
        std::mutex sampleMutex;
        std::vector<uint64_t> latenciesUs;

        uint64_t t0 = BenchNowUs();
        const auto start = std::chrono::steady_clock::now();
        for (int32_t i = 0; i < targetOps; ++i) {
            auto due = start + std::chrono::microseconds((int64_t)(i + 1) * 1000000LL / (int64_t)targetOps);
            std::this_thread::sleep_until(due);
            uint64_t submitUs = BenchNowUs();
            mPool->SubmitTask([&done, &sampleMutex, &latenciesUs, submitUs]() {
                uint64_t latencyUs = BenchNowUs() - submitUs;
                {
                    std::lock_guard<std::mutex> lock(sampleMutex);
                    latenciesUs.push_back(latencyUs);
                }
                done.fetch_add(1);
            });
        }

        bool drained = BenchWaitUntil([&]() { return done.load() >= targetOps; }, THREADPOOL_DRAIN_TIMEOUT_MS);
        int32_t success = done.load();
        if (success > targetOps) {
            success = targetOps;
        }

        int32_t fail = targetOps - success;
        uint64_t elapsedUs = BenchNowUs() - t0;
        double lossRate = (double)fail / (double)targetOps;

        if (!latenciesUs.empty()) {
            BenchRecordByTimes("ThreadPool", scenario, latenciesUs, (uint64_t)success, lossRate,
                               "attempt=" + std::to_string(targetOps)
                               + ",success=" + std::to_string(success)
                               + ",fail=" + std::to_string(fail)
                               + ",drained=" + std::to_string(drained ? 1 : 0),
                               elapsedUs > 0 ? (double)success * 1000000.0 / (double)elapsedUs : 0.0);
        } else {
            BenchResult r = {0, 0, 0, 0};
            BenchRecordBySummary("ThreadPool", scenario, r,
                                 elapsedUs > 0 ? (double)success * 1000000.0 / (double)elapsedUs : 0.0,
                                 lossRate,
                                 "attempt=" + std::to_string(targetOps)
                                 + ",success=" + std::to_string(success)
                                 + ",fail=" + std::to_string(fail)
                                 + ",drained=" + std::to_string(drained ? 1 : 0));
        }

        BenchLog("ThreadPool %s: attempt=%d success=%d fail=%d drained=%d loss=%.2f%% avg=%.1f us/op rate=%.1f ops/s",
                 scenario, targetOps, success, fail, drained ? 1 : 0, lossRate * 100.0,
                 !latenciesUs.empty() ? (double)std::accumulate(latenciesUs.begin(), latenciesUs.end(), 0ULL) / (double)latenciesUs.size() : 0.0,
                 elapsedUs > 0 ? (double)success * 1000000.0 / (double)elapsedUs : 0.0);
    }

    SprThreadPool* mPool;
};

TEST_P(Core_ThreadPoolBench, Load1sOps) {
    const int32_t targetOps = GetParam();
    const std::string scenario = "Load1s" + std::to_string(targetOps) + "Ops";
    RunLoad1sScenario(scenario.c_str(), targetOps);
}

INSTANTIATE_TEST_SUITE_P(
    Load,
    Core_ThreadPoolBench,
    ::testing::Values(200, 500, 1000, 10000),
    [](const ::testing::TestParamInfo<int32_t>& info) {
        return std::to_string(info.param);
    }
);
