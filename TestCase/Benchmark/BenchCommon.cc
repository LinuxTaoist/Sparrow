/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BenchCommon.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Shared benchmark helpers and report output
 *  @date       : 2026/09/11
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/11 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 */

#include <fstream>
#include <sstream>
#include <algorithm>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <thread>
#include <chrono>
#include "gtest/gtest.h"
#include "BenchCommon.h"

namespace {

std::vector<BenchCaseResult>& BenchStore() {
    static std::vector<BenchCaseResult> store;
    return store;
}

uint64_t ParseUintFromNote(const std::string& note, const std::string& keyName) {
    const std::string key = keyName + "=";
    size_t pos = note.find(key);
    if (pos == std::string::npos) {
        return 0;
    }

    pos += key.size();
    if (pos >= note.size() || note[pos] < '0' || note[pos] > '9') {
        return 0;
    }

    uint64_t value = 0;
    while (pos < note.size() && note[pos] >= '0' && note[pos] <= '9') {
        value = value * 10 + (uint64_t)(note[pos] - '0');
        ++pos;
    }
    return value;
}

std::string FormatCountDisplay(const BenchCaseResult& r) {
    uint64_t attempt = ParseUintFromNote(r.note, "attempt");
    if (attempt == 0 && (r.scenario == "LimitProbeSummary" || r.scenario == "FindLimitSummary")) {
        attempt = ParseUintFromNote(r.note, "best_n");
        if (attempt == 0) {
            attempt = ParseUintFromNote(r.note, "first_exceed_n");
        }
    }
    if (attempt == 0) {
        attempt = r.count;
    }
    return std::to_string(r.count) + "/" + std::to_string(attempt);
}

std::string CurrentTimeString() {
    time_t now = time(nullptr);
    struct tm tmv;
    localtime_r(&now, &tmv);
    char buf[64] = {0};
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmv);
    return std::string(buf);
}

std::string ToAbsolutePath(const std::string& path) {
    if (path.empty()) {
        return path;
    }

    if (path[0] == '/') {
        return path;
    }

    char cwd[4096] = {0};
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        return path;
    }

    return std::string(cwd) + "/" + path;
}

void PrintSummaryToTerminal() {
    const std::vector<BenchCaseResult>& store = BenchStore();
    BenchLog("=================================================================================================");
    BenchLog("Sparrow Benchmark Summary");
    BenchLog("Run At: %s", CurrentTimeString().c_str());
    BenchLog("%-12s %-28s %13s %12s %8s %8s %8s %8s %8s",
             "Module", "Scenario", "Count", "Thpt/s", "Avg(us)", "P99(us)", "Min(us)", "Max(us)", "Loss(%)");
    for (size_t i = 0; i < store.size(); ++i) {
        const BenchCaseResult& r = store[i];
        std::string countDisplay = FormatCountDisplay(r);
        BenchLog("%-12s %-28s %13s %12.1f %8llu %8llu %8llu %8llu %8.4f",
                 r.module.c_str(), r.scenario.c_str(),
                 countDisplay.c_str(), r.throughput,
                 (unsigned long long)r.avgUs, (unsigned long long)r.p99Us,
                 (unsigned long long)r.minUs, (unsigned long long)r.maxUs,
                 r.lossRate * 100.0);
    }
    BenchLog("=================================================================================================");
}

void WriteTxtReport() {
    const std::vector<BenchCaseResult>& store = BenchStore();
    std::string path = BenchGetReportPath();
    std::ofstream ofs(path.c_str(), std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        BenchLog("Failed to open benchmark report path: %s", path.c_str());
        return;
    }

    ofs << "Sparrow Benchmark Report\n";
    ofs << "========================================================================================================================\n";
    ofs << "Generated : " << CurrentTimeString() << "\n";
    ofs << "Modules   : all\n";
    ofs << "Cases     : " << store.size() << "\n";
    ofs << "Unit      : latency(us), throughput(ops/s), loss(%)\n\n";

    ofs << "+--------------+------------------------------+---------------+------------+---------+---------+---------+---------+---------+---------+---------+\n";
    ofs << "| Module       | Scenario                     | Count(x/z)    | Thpt(op/s) | Avg(us) | P50(us) | P90(us) | P99(us) | Min(us) | Max(us) | Loss(%) |\n";
    ofs << "+--------------+------------------------------+---------------+------------+---------+---------+---------+---------+---------+---------+---------+\n";

    char line[512] = {0};
    for (size_t i = 0; i < store.size(); ++i) {
        const BenchCaseResult& r = store[i];
        std::string countDisplay = FormatCountDisplay(r);
        snprintf(line, sizeof(line),
             "| %-12s | %-28s | %13s | %10.1f | %7llu | %7llu | %7llu | %7llu | %7llu | %7llu | %7.4f |\n",
                 r.module.c_str(), r.scenario.c_str(),
             countDisplay.c_str(), r.throughput,
                 (unsigned long long)r.avgUs, (unsigned long long)r.p50Us,
                 (unsigned long long)r.p90Us, (unsigned long long)r.p99Us,
                 (unsigned long long)r.minUs, (unsigned long long)r.maxUs,
                 r.lossRate * 100.0);
        ofs << line;
        if (!r.note.empty()) {
            ofs << "  note: " << r.note << "\n";
        }
    }
    ofs << "+--------------+------------------------------+---------------+------------+---------+---------+---------+---------+---------+---------+---------+\n";
    ofs << "\n";
    ofs << "Report Path: " << path << "\n";
    ofs.close();
    BenchLog("Benchmark txt report updated: %s", path.c_str());
}

class BenchListener : public ::testing::EmptyTestEventListener {
public:
    void OnTestProgramEnd(const ::testing::UnitTest&) override {
        PrintSummaryToTerminal();
        WriteTxtReport();
    }
};

bool gListenerInstalled = false;
bool gEnvInstalled = false;
std::function<void()> gGlobalSetUpHook;
std::function<void()> gGlobalTearDownHook;

class BenchGlobalEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        if (gGlobalSetUpHook) {
            gGlobalSetUpHook();
        }
    }

    void TearDown() override {
        if (gGlobalTearDownHook) {
            gGlobalTearDownHook();
        }
    }
};

} // namespace

uint64_t BenchNowUs() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

uint64_t BenchPercentile(const std::vector<uint64_t>& sortedTimes, int permille) {
    if (sortedTimes.empty()) return 0;
    size_t idx = (size_t)((sortedTimes.size() - 1) * (uint64_t)permille / 1000);
    if (idx >= sortedTimes.size()) idx = sortedTimes.size() - 1;
    return sortedTimes[idx];
}

bool BenchWaitUntil(const std::function<bool()>& condition,
                    int32_t timeoutMs,
                    int32_t stepMs) {
    if (stepMs <= 0) {
        stepMs = 1;
    }

    int32_t elapsedMs = 0;
    while (elapsedMs < timeoutMs) {
        if (condition()) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(stepMs));
        elapsedMs += stepMs;
    }
    return condition();
}

BenchResult RunBench(const std::function<void()>& fn, int iterations) {
    std::vector<uint64_t> times((size_t)iterations);
    for (int i = 0; i < iterations; ++i) {
        uint64_t t0 = BenchNowUs();
        fn();
        times[(size_t)i] = BenchNowUs() - t0;
    }

    std::sort(times.begin(), times.end());
    uint64_t sum = 0;
    for (size_t i = 0; i < times.size(); ++i) sum += times[i];

    int skip = iterations / 20;
    if (skip * 2 >= iterations) skip = 0;
    uint64_t trimmedSum = 0;
    for (int i = skip; i < iterations - skip; ++i) {
        trimmedSum += times[(size_t)i];
    }
    uint64_t denom = (uint64_t)(iterations - 2 * skip);
    if (denom == 0) denom = (uint64_t)iterations;
    return {times[(size_t)skip], times[(size_t)(iterations - 1 - skip)], trimmedSum / denom, (uint64_t)iterations};
}

void BenchLog(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf("[   INFO   ] ");
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}

void BenchInstallReportListener() {
    if (gListenerInstalled) {
        return;
    }
    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new BenchListener());
    gListenerInstalled = true;
}

void BenchInstallGlobalEnvironment() {
    if (gEnvInstalled) {
        return;
    }
    ::testing::AddGlobalTestEnvironment(new BenchGlobalEnvironment());
    gEnvInstalled = true;
}

void BenchPrintModuleSummary(const std::string& module) {
    const std::vector<BenchCaseResult>& store = BenchStore();
    BenchLog("-------------------------------- Module Summary: %s --------------------------------", module.c_str());
    BenchLog("%-28s %13s %12s %8s %8s %8s %8s %8s",
             "Scenario", "Count", "Thpt/s", "Avg(us)", "P99(us)", "Min(us)", "Max(us)", "Loss(%)");

    bool found = false;
    for (size_t i = 0; i < store.size(); ++i) {
        const BenchCaseResult& r = store[i];
        if (r.module != module) {
            continue;
        }
        found = true;
        std::string countDisplay = FormatCountDisplay(r);
        BenchLog("%-28s %13s %12.1f %8llu %8llu %8llu %8llu %8.4f",
                 r.scenario.c_str(),
             countDisplay.c_str(),
                 r.throughput,
                 (unsigned long long)r.avgUs,
                 (unsigned long long)r.p99Us,
                 (unsigned long long)r.minUs,
                 (unsigned long long)r.maxUs,
                 r.lossRate * 100.0);
    }

    if (!found) {
        BenchLog("(no benchmark records for this module)");
    }
    BenchLog("--------------------------------------------------------------------------------------------");
}

void BenchSetGlobalHooks(const std::function<void()>& setUpFn,
                         const std::function<void()>& tearDownFn) {
    gGlobalSetUpHook = setUpFn;
    gGlobalTearDownHook = tearDownFn;
}

int BenchGetEnvInt(const char* key, int defaultValue) {
    const char* v = getenv(key);
    if (v == nullptr || v[0] == '\0') return defaultValue;
    int parsed = atoi(v);
    return parsed > 0 ? parsed : defaultValue;
}

std::string BenchGetReportPath() {
    mkdir("BenchmarkReport", 0755);
    return ToAbsolutePath(std::string("BenchmarkReport/benchmark_latest.txt"));
}

bool BenchShouldRunModule(const std::string&) {
    return true;
}

void BenchRecordByTimes(const std::string& module,
                        const std::string& scenario,
                        const std::vector<uint64_t>& times,
                        uint64_t count,
                        double lossRate,
                        const std::string& note,
                        double throughputOverride) {
    if (times.empty()) return;
    std::vector<uint64_t> sorted = times;
    std::sort(sorted.begin(), sorted.end());
    uint64_t sum = 0;
    for (size_t i = 0; i < sorted.size(); ++i) sum += sorted[i];
    double elapsedUs = (double)sum;

    BenchCaseResult r;
    r.module = module;
    r.scenario = scenario;
    r.count = count;
    r.throughput = throughputOverride >= 0.0
                 ? throughputOverride
                 : (elapsedUs > 0.0 ? (double)count * 1000000.0 / elapsedUs : 0.0);
    r.avgUs = sum / sorted.size();
    r.minUs = sorted.front();
    r.p50Us = BenchPercentile(sorted, 500);
    r.p90Us = BenchPercentile(sorted, 900);
    r.p99Us = BenchPercentile(sorted, 990);
    r.maxUs = sorted.back();
    r.lossRate = lossRate;
    r.note = note;
    BenchStore().push_back(r);
}

void BenchRecordBySummary(const std::string& module,
                          const std::string& scenario,
                          const BenchResult& br,
                          double throughput,
                          double lossRate,
                          const std::string& note) {
    BenchCaseResult r;
    r.module = module;
    r.scenario = scenario;
    r.count = br.count;
    r.throughput = throughput;
    r.avgUs = br.avgUs;
    r.minUs = br.minUs;
    r.p50Us = br.avgUs > 0 ? br.avgUs : br.minUs;
    r.p90Us = br.avgUs > 0 ? br.avgUs : br.minUs;
    r.p99Us = br.avgUs > 0 ? br.avgUs : br.minUs;
    r.maxUs = br.maxUs;
    r.lossRate = lossRate;
    r.note = note;
    BenchStore().push_back(r);
}
