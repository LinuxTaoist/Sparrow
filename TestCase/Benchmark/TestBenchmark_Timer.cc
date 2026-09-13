/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestBenchmark_Timer.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Timer benchmark cases
 *  @date       : 2026/09/11
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/11 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 */

#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <mutex>
#include <functional>
#include <string>
#include "gtest/gtest.h"
#include "BenchCommon.h"
#include "SprEpollSchedule.h"
#include "SprObserverWithMQueue.h"

namespace {
class BenchTimerObserver : public SprObserverWithMQueue {
public:
    explicit BenchTimerObserver(uint16_t idBase)
        : SprObserverWithMQueue((ModuleIDType)(InternalDefs::MODULE_PUBLIC_END + idBase),
                                "bench_timer",
                                InternalDefs::MEDIATOR_PROXY_MQUEUE),
          mTimerMsgId(0),
          mHitCount(0) {}

    void ResetRecords(uint32_t timerMsgId) {
        std::lock_guard<std::mutex> lock(mMutex);
        mTimerMsgId = timerMsgId;
        mHitTimesUs.clear();
        mHitCount.store(0);
    }

    int32_t GetHitCount() const {
        return mHitCount.load();
    }

    int32_t GetTotalCount() const {
        return mTotalCount.load();
    }

    std::vector<uint64_t> GetHitTimesUs() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mHitTimesUs;
    }

private:
    int32_t Init() override { return 0; }

    int32_t ProcessMsg(const SprMsg& msg) override {
        if (msg.GetMsgId() == mTimerMsgId) {
            mTotalCount.fetch_add(1);
            std::lock_guard<std::mutex> lock(mMutex);
            mHitTimesUs.push_back(BenchNowUs());
            mHitCount.fetch_add(1);
        }
        return 0;
    }

private:
    mutable std::mutex mMutex;
    std::vector<uint64_t> mHitTimesUs;
    uint32_t mTimerMsgId;
    std::atomic<int32_t> mHitCount;
    std::atomic<int32_t> mTotalCount { 0 };
};

class Core_TimerBench : public ::testing::Test {
protected:
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

        BenchPrintModuleSummary("Timer");
    }

    void SetUp() override {
        mObserver.Initialize();
    }

    void PrepareCaseState() {
        int32_t timeoutMs = 3000;
        int32_t stepMs = 5;
        int32_t quietMs = 50;
        int32_t lastTotal = mObserver.GetTotalCount();
        int32_t stableMs = 0;
        int32_t elapsedMs = 0;

        mObserver.UnregisterTimer(InternalDefs::SIG_ID_TEST_BENCHMARK_TIMER_EVENT);
        while (elapsedMs < timeoutMs) {
            std::this_thread::sleep_for(std::chrono::milliseconds(stepMs));
            elapsedMs += stepMs;

            int32_t curTotal = mObserver.GetTotalCount();
            if (curTotal == lastTotal) {
                stableMs += stepMs;
                if (stableMs >= quietMs) {
                    break;
                }
            } else {
                lastTotal = curTotal;
                stableMs = 0;
            }
        }

        mObserver.ResetRecords(InternalDefs::SIG_ID_TEST_BENCHMARK_TIMER_EVENT);
    }

    void RunTimerScenario(const char* scenario, int32_t periodMs, uint32_t repeatTimes) {
        const int32_t targetHits = (int32_t)repeatTimes;
        if (targetHits <= 0) {
            BenchResult r = {0, 0, 0, 0};
            BenchRecordBySummary("Timer", scenario, r, 0.0, 1.0,
                                 "attempt=0,success=0,invalid_param=repeat<=0"
                                 ",period_ms=" + std::to_string(periodMs));
            return;
        }

        std::vector<uint64_t> latenciesUs;
        latenciesUs.reserve((size_t)targetHits);

        int32_t timerRegFail = 0;
        int32_t timerWaitFail = 0;
        int32_t timerHitLoss = 0;

        PrepareCaseState();

        const uint64_t scenarioStartUs = BenchNowUs();

        int32_t progressEvery = std::min(std::max(targetHits / 4, 1), 20);
        const int32_t waitTimeoutMs = periodMs + (periodMs * 2) + 2000;

        BenchLog("Timer %s: start, attempt=%d, period=%d ms, progress_every=%d",
                 scenario, targetHits, periodMs, progressEvery);

        int32_t nextProgressHit = progressEvery;
        for (int32_t i = 0; i < targetHits; ++i) {
            mObserver.UnregisterTimer(InternalDefs::SIG_ID_TEST_BENCHMARK_TIMER_EVENT);
            mObserver.ResetRecords(InternalDefs::SIG_ID_TEST_BENCHMARK_TIMER_EVENT);

            uint64_t requestUs = BenchNowUs();
            int32_t regRet = mObserver.RegisterTimer(periodMs,
                                                     periodMs,
                                                     InternalDefs::SIG_ID_TEST_BENCHMARK_TIMER_EVENT,
                                                     1U);
            if (regRet != 0) {
                timerRegFail += 1;
                continue;
            }

            bool completed = BenchWaitUntil([&]() { return mObserver.GetHitCount() >= 1; }, waitTimeoutMs);
            mObserver.UnregisterTimer(InternalDefs::SIG_ID_TEST_BENCHMARK_TIMER_EVENT);

            if (!completed) {
                timerWaitFail += 1;
                continue;
            }

            std::vector<uint64_t> hitTimesUs = mObserver.GetHitTimesUs();
            if (hitTimesUs.empty()) {
                timerHitLoss += 1;
                continue;
            }

            uint64_t actualUs = hitTimesUs[0];
            latenciesUs.push_back(actualUs >= requestUs ? (actualUs - requestUs) : 0ULL);

            int32_t success = (int32_t)latenciesUs.size();
            if (progressEvery > 0
                && nextProgressHit > 0
                && nextProgressHit < targetHits
                && success >= nextProgressHit) {
                BenchLog("Timer %s progress: %d/%d", scenario, success, targetHits);
                nextProgressHit += progressEvery;
            }
        }

        int32_t S = (int32_t)latenciesUs.size();
        int32_t fail = targetHits - S;
        if (fail < 0) {
            fail = 0;
        }
        if (timerHitLoss < fail - timerRegFail - timerWaitFail) {
            timerHitLoss = fail - timerRegFail - timerWaitFail;
            if (timerHitLoss < 0) {
                timerHitLoss = 0;
            }
        }

        const uint64_t scenarioElapsedUs = BenchNowUs() - scenarioStartUs;

        if (latenciesUs.empty()) {
            BenchResult r = {0, 0, 0, 0};
            BenchRecordBySummary("Timer", scenario, r, 0.0, 1.0,
                                 "attempt=" + std::to_string(targetHits)
                                 + ",success=0"
                                 + ",timer_reg_fail=" + std::to_string(timerRegFail)
                                 + ",timer_wait_fail=" + std::to_string(timerWaitFail)
                                 + ",timer_hit_loss=" + std::to_string(timerHitLoss)
                                 + ",repeat=" + std::to_string(repeatTimes)
                                 + ",period_ms=" + std::to_string(periodMs));
            BenchLog("Timer %s: all failed, attempt=%d reg_fail=%d wait_fail=%d hit_loss=%d",
                     scenario, targetHits, timerRegFail, timerWaitFail, timerHitLoss);
            return;
        }

        std::sort(latenciesUs.begin(), latenciesUs.end());

        uint64_t sumErr = 0;
        for (size_t i = 0; i < latenciesUs.size(); ++i) {
            sumErr += latenciesUs[i];
        }

        double lossRate = (double)fail / (double)targetHits;
        double throughput = scenarioElapsedUs > 0
                          ? (double)S * 1000000.0 / (double)scenarioElapsedUs
                          : 0.0;
        BenchRecordByTimes("Timer", scenario, latenciesUs, (uint64_t)S, lossRate,
               "attempt=" + std::to_string(targetHits)
                   + ",success=" + std::to_string(S)
                   + ",timer_reg_fail=" + std::to_string(timerRegFail)
                   + ",timer_wait_fail=" + std::to_string(timerWaitFail)
                   + ",timer_hit_loss=" + std::to_string(timerHitLoss)
                   + ",repeat=" + std::to_string(repeatTimes)
                   + ",period_ms=" + std::to_string(periodMs)
                   + ",scenario_elapsed_us=" + std::to_string(scenarioElapsedUs),
                   throughput);
        BenchLog("Timer %s: attempt=%d success=%d reg_fail=%d wait_fail=%d hit_loss=%d loss=%.2f%% avg_lat=%llu us, p99_lat=%llu us, max_lat=%llu us, thpt=%.2f/s",
                                 scenario, targetHits, S, timerRegFail, timerWaitFail, timerHitLoss, lossRate * 100.0,
             (unsigned long long)(sumErr / (uint64_t)S),
             (unsigned long long)latenciesUs[(size_t)S * 99 / 100],
                 (unsigned long long)latenciesUs.back(),
                 throughput);
    }

protected:
    static std::thread sEpollThread;
    BenchTimerObserver mObserver { 330 };

};

std::thread Core_TimerBench::sEpollThread;

} // namespace

// 测试 500ms 周期定时器触发误差与丢失情况。
TEST_F(Core_TimerBench, Period500ms) {
    RunTimerScenario("500ms", 500, 20U);
}

// 测试 200ms 周期定时器触发误差与丢失情况。
TEST_F(Core_TimerBench, Period200ms) {
    RunTimerScenario("200ms", 200, 40U);
}

// 测试 1000ms 周期定时器触发误差与丢失情况。
TEST_F(Core_TimerBench, Period1000ms) {
    RunTimerScenario("1000ms", 1000, 8U);
}

// 测试 100ms 周期定时器触发误差与丢失情况。
TEST_F(Core_TimerBench, Period100ms) {
    RunTimerScenario("100ms", 100, 500U);
}
