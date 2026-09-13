# Benchmark

## 1. 这是做什么的
Benchmark 用来测主框架的性能和稳定性。

当前已覆盖：
- MQueue / MQueueThread：A 发送线程 + B 处理线程的 1 秒固定负载性能。
- ThreadPool：1 秒固定负载任务提交与完成能力。
- SprMsg：1 秒固定负载编解码往返能力。
- Log：1 秒固定负载格式化能力。
- Timer：定时器精度与误差（当前保持独立场景）。

统一标准场景（非 Timer）：
- Load1s200Ops
- Load1s500Ops
- Load1s1000Ops
- Load1s10000Ops

相关实现：
- [BenchCommon.h](BenchCommon.h)
- [BenchCommon.cc](BenchCommon.cc)
- [TestBenchmark_Log.cc](TestBenchmark_Log.cc)
- [TestBenchmark_Timer.cc](TestBenchmark_Timer.cc)
- [TestBenchmark_ThreadPool.cc](TestBenchmark_ThreadPool.cc)
- [TestBenchmark_MQueue.cc](TestBenchmark_MQueue.cc)
- [TestBenchmark_MQueueThread.cc](TestBenchmark_MQueueThread.cc)
- [TestBenchmark_SprMsg.cc](TestBenchmark_SprMsg.cc)

## 2. 怎么跑
先编译：

```bash
cd Platform/Default/Build
./build_default.sh test_benchmark
```

再运行：

```bash
cd Release/Default/Bin
./test_benchmark
```

只跑某个模块（推荐使用 gtest 原生过滤）：

```bash
./test_benchmark --gtest_filter=Core_TimerBench.*
```

常见示例：

```bash
# 只跑 MQueue 四档标准场景
./test_benchmark --gtest_filter=Core_SprObsMQBench.Load1s*OpsDualThread

# 只跑 MQueueThread 四档标准场景
./test_benchmark --gtest_filter=Core_SprObsMQThreadBench.Load1s*OpsDualThread

# 只跑非 Timer 的统一四档场景
./test_benchmark --gtest_filter=*Load1s200Ops*:*Load1s500Ops*:*Load1s1000Ops*:*Load1s10000Ops*

# 只跑 Timer
./test_benchmark --gtest_filter=Core_TimerBench.*
```

## 3. 怎么看结果
- 终端：每个模块结束会先打印该模块汇总；全部结束后再打印总汇总。
- 文本：默认覆盖写入 `BenchmarkReport/benchmark_latest.txt`。
- 若需自定义报告路径：设置 `SPARROW_BENCH_REPORT=/your/path/report.txt`。

## 4. 结果字段是什么意思
- Module：模块名。
- Scenario：测试场景名。
- Count：x/z，x 为有效样本数，z 为尝试样本数。
- Thpt(op/s)：有效样本吞吐，单位 次/秒；有效样本细项见 note。
- Avg(us)：平均耗时，单位微秒。
- P99(us)：99 分位耗时，单位微秒。
- Min(us) / Max(us)：最小 / 最大耗时，单位微秒。
- Loss(%)：丢失率。

