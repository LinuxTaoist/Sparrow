/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestBenchmark_SprMsg.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SprMsg benchmark cases
 *  @date       : 2026/09/11
 *---------------------------------------------------------------------------------------------------------------------
 */

#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <numeric>
#include "gtest/gtest.h"
#include "SprMsg.h"
#include "BenchCommon.h"

class Core_SprMsgBench : public testing::Test {
protected:
    static void SetUpTestSuite() {
        BenchInstallReportListener();
        BenchInstallGlobalEnvironment();
    }

    static void TearDownTestSuite() {
        BenchPrintModuleSummary("SprMsg");
    }

    void SetUp() override {
        msg.SetFrom(0x1234);
        msg.SetTo(0x5678);
        msg.SetMsgId(0xABCD);
        msg.SetU32Value(0xDEADBEEF);
        msg.SetString("benchmark payload string data");
        msg.SetU8Vec({0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08});
    }

    SprMsg msg;
};

void RunLoad1sScenario(const char* scenario, int32_t targetOps, SprMsg& msg) {
    int32_t encodeFail = 0;
    int32_t decodeFail = 0;
    int32_t dataFail = 0;
    int32_t success = 0;
    std::vector<uint64_t> sampleUs;
    sampleUs.reserve((size_t)targetOps);

    uint64_t t0 = BenchNowUs();
    const auto start = std::chrono::steady_clock::now();
    for (int32_t i = 0; i < targetOps; ++i) {
        auto due = start + std::chrono::microseconds((int64_t)(i + 1) * 1000000LL / (int64_t)targetOps);
        std::this_thread::sleep_until(due);

        msg.SetU32Value((uint32_t)i);
        msg.SetString("benchmark payload string data #" + std::to_string(i));

        uint64_t opStartUs = BenchNowUs();
        std::string out;
        if (msg.Encode(out) != 0) {
            ++encodeFail;
            continue;
        }

        SprMsg dec;
        if (dec.Decode(out) != 0) {
            ++decodeFail;
            continue;
        }

        if (dec.GetFrom() != msg.GetFrom() || dec.GetTo() != msg.GetTo()
            || dec.GetMsgId() != msg.GetMsgId() || dec.GetU32Value() != msg.GetU32Value()
            || dec.GetString() != msg.GetString()) {
            ++dataFail;
            continue;
        }
        sampleUs.push_back(BenchNowUs() - opStartUs);
        ++success;
    }

    uint64_t elapsedUs = BenchNowUs() - t0;
    double lossRate = (double)(targetOps - success) / (double)targetOps;

    if (!sampleUs.empty()) {
        BenchRecordByTimes("SprMsg", scenario, sampleUs, (uint64_t)success, lossRate,
                           "attempt=" + std::to_string(targetOps)
                           + ",success=" + std::to_string(success)
                           + ",encode_fail=" + std::to_string(encodeFail)
                           + ",decode_fail=" + std::to_string(decodeFail)
                           + ",data_fail=" + std::to_string(dataFail),
                           elapsedUs > 0 ? (double)success * 1000000.0 / (double)elapsedUs : 0.0);
    } else {
        BenchResult r = {0, 0, 0, 0};
        BenchRecordBySummary("SprMsg", scenario, r,
                             elapsedUs > 0 ? (double)success * 1000000.0 / (double)elapsedUs : 0.0,
                             lossRate,
                             "attempt=" + std::to_string(targetOps)
                             + ",success=" + std::to_string(success)
                             + ",encode_fail=" + std::to_string(encodeFail)
                             + ",decode_fail=" + std::to_string(decodeFail)
                             + ",data_fail=" + std::to_string(dataFail));
    }

    BenchLog("SprMsg %s: attempt=%d success=%d encode_fail=%d decode_fail=%d data_fail=%d loss=%.2f%% avg=%.1f us/op rate=%.1f ops/s",
             scenario, targetOps, success, encodeFail, decodeFail, dataFail, lossRate * 100.0,
             !sampleUs.empty() ? (double)std::accumulate(sampleUs.begin(), sampleUs.end(), 0ULL) / (double)sampleUs.size() : 0.0,
             elapsedUs > 0 ? (double)success * 1000000.0 / (double)elapsedUs : 0.0);
}

// 测试 1 秒内发送 200 条消息的吞吐和丢包情况
TEST_F(Core_SprMsgBench, Load1s200Ops) {
    RunLoad1sScenario("Load1s200Ops", 200, msg);
}

// 测试 1 秒内发送 500 条消息的吞吐和丢包情况
TEST_F(Core_SprMsgBench, Load1s500Ops) {
    RunLoad1sScenario("Load1s500Ops", 500, msg);
}

// 测试 1 秒内发送 1000 条消息的吞吐和丢包情况
TEST_F(Core_SprMsgBench, Load1s1000Ops) {
    RunLoad1sScenario("Load1s1000Ops", 1000, msg);
}

// 测试 1 秒内发送 10000 条消息的吞吐和丢包情况
TEST_F(Core_SprMsgBench, Load1s10000Ops) {
    RunLoad1sScenario("Load1s10000Ops", 10000, msg);
}
