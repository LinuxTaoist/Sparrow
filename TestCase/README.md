# 测试指南
- BenchMark: 基准测试，用于测试性能，如消息处理速度、线程池处理速度等。

- Internal: 基于内部所有源码的测试，可访问所有工程源码，类似于白盒测试。

- External: 基于外部释放头文件和库测试，仅能够访问释放的文件，类似于黑盒测试。

## 当前测试版本
- googletest 版本: 1.12.1
- 来源: 3rdParty/googletest/googletest-1.12.1.tar.gz

## 启动编译
参考 [googletest集成指南](../3rdParty/googletest/README.md)。

## 覆盖率测试（Default Ubuntu）

### 说明
- 覆盖率默认关闭，不会在常规编译时自动生成。
- 覆盖率只在 Default(Ubuntu) 平台执行。
- 当前覆盖率目录只保留 1 个脚本：`Tools/testcoverage/gen_coverage_report.sh`。
- 该脚本只负责“生成报告”，不会执行测试用例。

### 复现步骤（按顺序执行）

1) 打开覆盖率开关
编辑 `Platform/Default/Build/Options/Default_modules_config.cmake`，确认：

		set(BUILD_TESTCASE ON)
		set(BUILD_COVERAGE ON)

2) 编译测试程序
在项目根目录执行：

		cd Platform/Default/Build
		./build_default.sh

预期结果：生成 `Release/Default/Bin/test_external`（以及其他测试二进制）。

3) 运行测试，产生 .gcda
在项目根目录执行（示例）：

		Release/Default/Bin/test_external

可选方式（在构建目录执行）：

		ctest --output-on-failure

预期结果：在 `Release/Default/Cache` 下出现 `.gcda` 文件。

4) 生成覆盖率报告（唯一脚本）
在项目根目录执行：

		./Tools/testcoverage/gen_coverage_report.sh

若当前环境没有 `lcov/genhtml`，终端会直接提示文本报告生成路径。

可选：指定 gcov 工具

		./Tools/testcoverage/gen_coverage_report.sh --gcov-tool gcov

5) 查看报告
- 若系统有 lcov/genhtml：

		Release/Default/Coverage/html/index.html

- 若系统没有 lcov/genhtml（自动回退）：

		Release/Default/Coverage/coverage_summary.txt

### 4) 测试目标
开启 BUILD_TESTCASE 后，已注册以下测试目标：
- test_internal
- test_external
- test_benchmark