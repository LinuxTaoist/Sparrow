/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestBenchmark_Log.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Logging-front benchmark cases
 *  @date       : 2026/09/11
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/11 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 */

#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include "gtest/gtest.h"
#include "BenchCommon.h"
#include "CoreTypeDefs.h"
#define LOG_TAG "LogBench"
#include "SprLog.h"

namespace {
constexpr int32_t LOG_BENCH_PROC_COUNT = 4;
constexpr int32_t LOG_BENCH_THREAD_PER_PROC = 4;

struct ChildPayloadHeader {
    int32_t success;
    int32_t sampleCount;
};

bool WriteAll(int fd, const void* buf, size_t len) {
    const char* p = static_cast<const char*>(buf);
    size_t done = 0;
    while (done < len) {
        ssize_t n = write(fd, p + done, len - done);
        if (n <= 0) {
            return false;
        }
        done += (size_t)n;
    }
    return true;
}

bool ReadAll(int fd, void* buf, size_t len) {
    char* p = static_cast<char*>(buf);
    size_t done = 0;
    while (done < len) {
        ssize_t n = read(fd, p + done, len - done);
        if (n <= 0) {
            return false;
        }
        done += (size_t)n;
    }
    return true;
}

class Core_LogBench : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        BenchInstallReportListener();
        BenchInstallGlobalEnvironment();
        SprLog::GetInstance()->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);
    }

    static void TearDownTestSuite() {
        BenchPrintModuleSummary("Log");
    }

    void SetUp() override {
        mBuf.assign(256, '\0');
    }

    void TearDown() override {
        mBuf.clear();
    }

    void RunLoad1sScenario(const char* scenario, int32_t targetOps) {
        if (targetOps <= 0) {
            BenchResult r = {0, 0, 0, 0};
            BenchRecordBySummary("Log", scenario, r, 0.0, 1.0, "attempt=0,success=0,invalid_target=1");
            return;
        }

        int32_t logFail = 0;
        int32_t success = 0;
        std::vector<uint64_t> sampleUs;
        sampleUs.reserve((size_t)targetOps);
        uint64_t t0 = BenchNowUs();

        const int32_t procCount = LOG_BENCH_PROC_COUNT;
        const int32_t threadPerProc = LOG_BENCH_THREAD_PER_PROC;
        const int32_t workerCount = procCount * threadPerProc;
        uint64_t startEpochUs = BenchNowUs() + 200000ULL;

        std::vector<pid_t> pids;
        pids.reserve((size_t)procCount);
        std::vector<int> readFds;
        readFds.reserve((size_t)procCount);

        for (int32_t p = 0; p < procCount; ++p) {
            int pipefd[2] = {-1, -1};
            if (pipe(pipefd) != 0) {
                logFail += targetOps;
                break;
            }

            pid_t pid = fork();
            if (pid < 0) {
                close(pipefd[0]);
                close(pipefd[1]);
                logFail += targetOps;
                break;
            }

            if (pid == 0) {
                close(pipefd[0]);

                std::mutex mergeMutex;
                std::vector<uint64_t> localSamples;
                localSamples.reserve((size_t)(targetOps / procCount + 8));
                int32_t localSuccess = 0;

                std::vector<std::thread> workers;
                workers.reserve((size_t)threadPerProc);
                for (int32_t t = 0; t < threadPerProc; ++t) {
                    const int32_t workerId = p * threadPerProc + t;
                    workers.emplace_back([&, workerId]() {
                        std::vector<uint64_t> threadSamples;
                        threadSamples.reserve((size_t)(targetOps / workerCount + 8));
                        int32_t threadSuccess = 0;

                        for (int32_t i = workerId; i < targetOps; i += workerCount) {
                            uint64_t dueUs = startEpochUs + (uint64_t)(i + 1) * 1000000ULL / (uint64_t)targetOps;
                            uint64_t nowUs = BenchNowUs();
                            if (dueUs > nowUs) {
                                std::this_thread::sleep_for(std::chrono::microseconds((int64_t)(dueUs - nowUs)));
                            }

                            uint64_t opStartUs = BenchNowUs();
                            SPR_LOGI("log idx=%d level=%s value=%d text=%s",
                                     i, "INFO", i * 3 + 7, "benchmark_load1s");
                            threadSamples.push_back(BenchNowUs() - opStartUs);
                            ++threadSuccess;
                        }

                        std::lock_guard<std::mutex> lk(mergeMutex);
                        localSuccess += threadSuccess;
                        localSamples.insert(localSamples.end(), threadSamples.begin(), threadSamples.end());
                    });
                }

                for (size_t i = 0; i < workers.size(); ++i) {
                    workers[i].join();
                }

                ChildPayloadHeader hdr = {
                    localSuccess,
                    (int32_t)localSamples.size()
                };

                bool ok = WriteAll(pipefd[1], &hdr, sizeof(hdr));
                if (ok && !localSamples.empty()) {
                    ok = WriteAll(pipefd[1], &localSamples[0], localSamples.size() * sizeof(uint64_t));
                }
                close(pipefd[1]);
                _exit(ok ? 0 : 2);
            }

            close(pipefd[1]);
            pids.push_back(pid);
            readFds.push_back(pipefd[0]);
        }

        for (size_t i = 0; i < readFds.size(); ++i) {
            ChildPayloadHeader hdr = {0, 0};
            bool ok = ReadAll(readFds[i], &hdr, sizeof(hdr));
            if (ok && hdr.sampleCount > 0) {
                std::vector<uint64_t> childSamples((size_t)hdr.sampleCount);
                ok = ReadAll(readFds[i], &childSamples[0], childSamples.size() * sizeof(uint64_t));
                if (ok) {
                    sampleUs.insert(sampleUs.end(), childSamples.begin(), childSamples.end());
                }
            }

            close(readFds[i]);
            if (ok) {
                success += hdr.success;
            } else {
                logFail += targetOps;
            }
        }

        for (size_t i = 0; i < pids.size(); ++i) {
            int status = 0;
            if (waitpid(pids[i], &status, 0) <= 0 || !WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                logFail += targetOps;
            }
        }

        if (success > targetOps) {
            success = targetOps;
        }
        if ((int32_t)sampleUs.size() > success) {
            sampleUs.resize((size_t)success);
        }

        uint64_t elapsedUs = BenchNowUs() - t0;
        double lossRate = (double)(targetOps - success) / (double)targetOps;
        uint64_t sampleSumUs = 0;
        for (size_t i = 0; i < sampleUs.size(); ++i) {
            sampleSumUs += sampleUs[i];
        }
        double avgSampleUs = sampleUs.empty() ? 0.0 : (double)sampleSumUs / (double)sampleUs.size();
        if (sampleUs.empty()) {
            BenchResult r = {0, 0, 0, 0};
            BenchRecordBySummary("Log", scenario, r,
                                 elapsedUs > 0 ? (double)success * 1000000.0 / (double)elapsedUs : 0.0,
                                 lossRate,
                                 "attempt=" + std::to_string(targetOps)
                                 + ",success=" + std::to_string(success)
                                 + ",log_fail=" + std::to_string(logFail));
        } else {
            BenchRecordByTimes("Log", scenario, sampleUs, (uint64_t)success, lossRate,
                               "attempt=" + std::to_string(targetOps)
                               + ",success=" + std::to_string(success)
                               + ",log_fail=" + std::to_string(logFail),
                               elapsedUs > 0 ? (double)success * 1000000.0 / (double)elapsedUs : 0.0);
        }

        BenchLog("Log %s: attempt=%d success=%d log_fail=%d loss=%.2f%% avg=%.1f us/op rate=%.1f ops/s",
             scenario, targetOps, success, logFail, lossRate * 100.0,
                 avgSampleUs,
                 elapsedUs > 0 ? (double)success * 1000000.0 / (double)elapsedUs : 0.0);
    }

protected:
    std::string mBuf;
};

} // namespace

// 测试 1 秒内打印 200 条日志的吞吐和丢包情况
TEST_F(Core_LogBench, Load1s200Ops) {
    RunLoad1sScenario("Load1s200Ops", 200);
}

// 测试 1 秒内打印 500 条日志的吞吐和丢包情况
TEST_F(Core_LogBench, Load1s500Ops) {
    RunLoad1sScenario("Load1s500Ops", 500);
}

// 测试 1 秒内打印 1000 条日志的吞吐和丢包情况
TEST_F(Core_LogBench, Load1s1000Ops) {
    RunLoad1sScenario("Load1s1000Ops", 1000);
}

// 测试 1 秒内打印 10000 条日志的吞吐和丢包情况
TEST_F(Core_LogBench, Load1s10000Ops) {
    RunLoad1sScenario("Load1s10000Ops", 10000);
}
