/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BenchCommon.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Shared benchmark helpers and reporting interfaces
 *  @date       : 2026/09/11
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/11 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 */

#ifndef __BENCH_COMMON_H__
#define __BENCH_COMMON_H__

#include <stdint.h>
#include <functional>
#include <string>
#include <vector>

struct BenchResult {
    uint64_t minUs;
    uint64_t maxUs;
    uint64_t avgUs;
    uint64_t count;
};

struct BenchCaseResult {
    std::string module;
    std::string scenario;
    uint64_t count;
    double throughput;
    uint64_t avgUs;
    uint64_t minUs;
    uint64_t p50Us;
    uint64_t p90Us;
    uint64_t p99Us;
    uint64_t maxUs;
    double lossRate;
    std::string note;
};

uint64_t BenchNowUs();
uint64_t BenchPercentile(const std::vector<uint64_t>& sortedTimes, int permille);
bool BenchWaitUntil(const std::function<bool()>& condition,
                    int32_t timeoutMs,
                    int32_t stepMs = 1);

BenchResult RunBench(const std::function<void()>& fn, int iterations);

int BenchGetEnvInt(const char* key, int defaultValue);

bool BenchShouldRunModule(const std::string& module);

std::string BenchGetReportPath();

void BenchLog(const char* fmt, ...);
void BenchInstallReportListener();
void BenchInstallGlobalEnvironment();
void BenchPrintModuleSummary(const std::string& module);
void BenchSetGlobalHooks(const std::function<void()>& setUpFn,
                         const std::function<void()>& tearDownFn);

void BenchRecordByTimes(const std::string& module,
                        const std::string& scenario,
                        const std::vector<uint64_t>& times,
                        uint64_t count,
                        double lossRate,
                        const std::string& note,
                        double throughputOverride = -1.0);

void BenchRecordBySummary(const std::string& module,
                          const std::string& scenario,
                          const BenchResult& br,
                          double throughput,
                          double lossRate,
                          const std::string& note);

#endif // __BENCH_COMMON_H__
