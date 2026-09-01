/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestBenchmark.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Core module performance benchmarks
 *  @date       : 2026/08/10
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/08/10 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdio.h>
#include <string>
#include <vector>
#include <atomic>
#include <algorithm>
#include <unistd.h>
#include <inttypes.h>
#include <sys/time.h>
#include "SprMsg.h"
#include "SprObserverWithMQueue.h"
#include "SprThreadPool.h"
#include "RunningTiming.h"
#include "gtest/gtest.h"

using namespace InternalDefs;

#define TEST_LOG(fmt, args...) printf("[   INFO   ] " fmt "\n", ##args)

static uint64_t NowUs() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

struct BenchResult {
    uint64_t minUs;
    uint64_t maxUs;
    uint64_t avgUs;
    uint64_t count;
};

static BenchResult RunBench(std::function<void()> fn, int iterations) {
    std::vector<uint64_t> times(iterations);
    for (int i = 0; i < iterations; ++i) {
        uint64_t t0 = NowUs();
        fn();
        times[i] = NowUs() - t0;
    }
    std::sort(times.begin(), times.end());
    uint64_t sum = 0;
    for (auto t : times) sum += t;
    int skip = iterations / 20;
    uint64_t trimmedSum = 0;
    for (int i = skip; i < iterations - skip; ++i) trimmedSum += times[i];
    return {times[skip], times[iterations - 1 - skip], trimmedSum / (iterations - 2 * skip), (uint64_t)iterations};
}

namespace {
class BenchObserver : public SprObserverWithMQueue {
public:
    BenchObserver(uint16_t idBase)
        : SprObserverWithMQueue((ModuleIDType)(MODULE_PUBLIC_END + idBase), "bench", MEDIATOR_PROXY_MQUEUE) {}
    int32_t Init() override { return 0; }
    int32_t ProcessMsg(const SprMsg& msg) override { return 0; }
    int32_t Recv(SprMsg& msg) { return RecvMsg(msg); }
};
} // anonymous namespace

class Core_SprMsgBench : public ::testing::Test {
protected:
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

class Core_ThreadPoolBench : public ::testing::Test {
protected:
    void SetUp() override { mPool = SprThreadPool::GetInstance(); }
    SprThreadPool* mPool;
};

// ====================================================================================================================
// SprMsg encode/decode benchmarks
// ====================================================================================================================

TEST_F(Core_SprMsgBench, EncodeThroughput) {
    const int N = 10000;
    BenchResult r = RunBench([this]() {
        std::string out;
        msg.Encode(out);
    }, N);
    TEST_LOG("SprMsg Encode: %d iterations, avg=%llu us/op, min=%llu us, max=%llu us",
             (int)r.count, (unsigned long long)r.avgUs, (unsigned long long)r.minUs, (unsigned long long)r.maxUs);
}

TEST_F(Core_SprMsgBench, DecodeThroughput) {
    std::string encoded;
    msg.Encode(encoded);
    const int N = 10000;
    BenchResult r = RunBench([this, &encoded]() {
        SprMsg dec;
        dec.Decode(const_cast<std::string&>(encoded));
    }, N);
    TEST_LOG("SprMsg Decode: %d iterations, avg=%llu us/op, min=%llu us, max=%llu us",
             (int)r.count, (unsigned long long)r.avgUs, (unsigned long long)r.minUs, (unsigned long long)r.maxUs);
}

TEST_F(Core_SprMsgBench, EncodeDecodeRoundtripThroughput) {
    const int N = 10000;
    BenchResult r = RunBench([this]() {
        std::string out;
        msg.Encode(out);
        SprMsg dec;
        dec.Decode(out);
    }, N);
    TEST_LOG("SprMsg Roundtrip: %d iterations, avg=%llu us/op, min=%llu us, max=%llu us",
             (int)r.count, (unsigned long long)r.avgUs, (unsigned long long)r.minUs, (unsigned long long)r.maxUs);
}

TEST_F(Core_SprMsgBench, LargeMessageEncodeDecode) {
    std::string bigStr(1024, 'X');
    std::vector<uint32_t> bigVec(256, 0xCAFEBABE);
    msg.SetString(bigStr);
    msg.SetU32Vec(bigVec);
    std::string encoded;
    msg.Encode(encoded);
    TEST_LOG("Large message encoded size: %zu bytes", encoded.size());

    const int N = 10000;
    BenchResult r = RunBench([this, &encoded]() {
        SprMsg dec;
        dec.Decode(const_cast<std::string&>(encoded));
    }, N);
    TEST_LOG("SprMsg Large(1KB+1KB): %d iterations, avg=%llu us/op, min=%llu us, max=%llu us",
             (int)r.count, (unsigned long long)r.avgUs, (unsigned long long)r.minUs, (unsigned long long)r.maxUs);
}

TEST_F(Core_SprMsgBench, EncodeLatencyPercentiles) {
    const int N = 10000;
    std::vector<uint64_t> times(N);
    for (int i = 0; i < N; ++i) {
        uint64_t t0 = NowUs();
        std::string out;
        msg.Encode(out);
        times[i] = NowUs() - t0;
    }
    std::sort(times.begin(), times.end());
    TEST_LOG("SprMsg Encode latency: P50=%llu us, P90=%llu us, P99=%llu us, P999=%llu us",
             (unsigned long long)times[N/2], (unsigned long long)times[N*9/10], (unsigned long long)times[N*99/100], (unsigned long long)times[N*999/1000]);
}

TEST_F(Core_SprMsgBench, DecodeLatencyPercentiles) {
    std::string encoded;
    msg.Encode(encoded);
    const int N = 10000;
    std::vector<uint64_t> times(N);
    for (int i = 0; i < N; ++i) {
        uint64_t t0 = NowUs();
        SprMsg dec;
        dec.Decode(const_cast<std::string&>(encoded));
        times[i] = NowUs() - t0;
    }
    std::sort(times.begin(), times.end());
    TEST_LOG("SprMsg Decode latency: P50=%llu us, P90=%llu us, P99=%llu us, P999=%llu us",
             (unsigned long long)times[N/2], (unsigned long long)times[N*9/10], (unsigned long long)times[N*99/100], (unsigned long long)times[N*999/1000]);
}

TEST_F(Core_SprMsgBench, RoundtripLatencyPercentiles) {
    const int N = 10000;
    std::vector<uint64_t> times(N);
    for (int i = 0; i < N; ++i) {
        uint64_t t0 = NowUs();
        std::string out;
        msg.Encode(out);
        SprMsg dec;
        dec.Decode(out);
        times[i] = NowUs() - t0;
    }
    std::sort(times.begin(), times.end());
    TEST_LOG("SprMsg Roundtrip latency: P50=%llu us, P90=%llu us, P99=%llu us, P999=%llu us",
             (unsigned long long)times[N/2], (unsigned long long)times[N*9/10], (unsigned long long)times[N*99/100], (unsigned long long)times[N*999/1000]);
}

// ====================================================================================================================
// SprThreadPool benchmarks
// ====================================================================================================================

TEST_F(Core_ThreadPoolBench, EmptyTaskDispatchLatency) {
    const int N = 1000;
    std::vector<uint64_t> latencies(N);
    for (int i = 0; i < N; ++i) {
        std::atomic<bool> done(false);
        uint64_t t0 = NowUs();
        mPool->SubmitTask([&done]() { done = true; });
        while (!done) { usleep(0); }
        latencies[i] = NowUs() - t0;
    }
    std::sort(latencies.begin(), latencies.end());
    uint64_t sum = 0;
    for (auto l : latencies) sum += l;
    TEST_LOG("ThreadPool empty task: avg=%llu us/op, p50=%llu us, p99=%llu us",
             (unsigned long long)(sum / N), (unsigned long long)latencies[N/2], (unsigned long long)latencies[N*99/100]);
}

TEST_F(Core_ThreadPoolBench, ConcurrentTaskThroughput) {
    const int kTasks = 500;
    std::atomic<int> count(0);
    RunningTiming timer;
    for (int i = 0; i < kTasks; ++i) {
        mPool->SubmitTask([&count]() { usleep(1000); count++; });
    }
    while (count < kTasks) { usleep(10000); }
    uint64_t elapsedUs = timer.GetElapsedTimeInMSec() * 1000;
    TEST_LOG("ThreadPool concurrent: %d tasks with 1ms work, total=%llu ms, rate=%.1f tasks/s",
             kTasks, (unsigned long long)(elapsedUs / 1000), (double)kTasks * 1000000 / elapsedUs);
}

// ====================================================================================================================
// SprObserverWithMQueue (Encode + MQueue Send/Recv + Decode) benchmarks
// ====================================================================================================================

TEST(Core_SprObsMQBench, SendRecvThroughput) {
    BenchObserver sender(1);
    BenchObserver receiver(2);

    SprMsg msg(0xBBBB);
    msg.SetU32Value(0xDEADBEEF);
    msg.SetString("benchmark payload string data");

    const int N = 500;
    uint64_t t0 = NowUs();
    for (int i = 0; i < N; ++i) {
        sender.SendMsg(msg);
        SprMsg dec;
        receiver.Recv(dec);
    }
    uint64_t elapsedUs = NowUs() - t0;

    TEST_LOG("SprObsMQ: %d Encode+Send+Recv+Decode pairs, avg=%.1f us/op, rate=%.0f msg/s",
             N, (double)elapsedUs / N, (double)N * 1000000 / elapsedUs);
}

TEST(Core_SprObsMQBench, SendRecvLatencyPercentiles) {
    BenchObserver sender(3);
    BenchObserver receiver(4);

    SprMsg msg(0xCCCC);
    msg.SetU32Value(0xCAFEBABE);
    msg.SetString("latency test");

    const int N = 500;
    std::vector<uint64_t> times(N);
    for (int i = 0; i < N; ++i) {
        uint64_t t0 = NowUs();
        sender.SendMsg(msg);
        SprMsg dec;
        receiver.Recv(dec);
        times[i] = NowUs() - t0;
    }
    std::sort(times.begin(), times.end());

    TEST_LOG("SprObsMQ latency: P50=%llu us, P90=%llu us, P99=%llu us, P999=%llu us",
             (unsigned long long)times[N/2], (unsigned long long)times[N*9/10], (unsigned long long)times[N*99/100], (unsigned long long)times[N*999/1000]);
}
