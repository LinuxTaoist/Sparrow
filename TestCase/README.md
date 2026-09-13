# TestCase

## 1. 这是做什么的
TestCase 是 Sparrow 的测试入口目录，包含三类测试：
- [Benchmark](Benchmark/README.md)：性能与稳定性测试。
- [Internal](Internal)：内部源码测试，偏白盒。
- [External](External)：对外接口测试，偏黑盒。

## 2. 怎么编译和运行
在工程根目录执行：

```bash
cd Platform/Default/Build
./build_default.sh
```

编译后常用测试程序在 [Release/Default/Bin](../Release/Default/Bin)：
- test_internal
- test_external
- test_benchmark

## 3. 怎么快速定位文档
- 基准测试说明： [TestCase/Benchmark/README.md](Benchmark/README.md)
- googletest 集成说明： [3rdParty/googletest/README.md](../3rdParty/googletest/README.md)

## 4. 覆盖率（Default 平台，可选）
先确认 [Platform/Default/Build/Options/Default_modules_config.cmake](../Platform/Default/Build/Options/Default_modules_config.cmake) 打开了 BUILD_TESTCASE 和 BUILD_COVERAGE。

生成覆盖率报告命令：

```bash
./Tools/gtestreport/gtest_coverage.sh --run-tests
```

结果位置：
- 有 lcov/genhtml 时：Release/Default/Coverage/html/index.html
- 无 lcov/genhtml 时：Release/Default/Coverage/coverage_summary.txt