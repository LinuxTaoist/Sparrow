# gtest 测试报告

## 1. 这是做什么的
提供两个脚本，分别生成 gtest 的**覆盖率报告**和**测试结果报告**。

| 脚本 | 用途 |
|------|------|
| `gtest_coverage.sh` | 采集代码覆盖率，生成 HTML 报告 |
| `gtest_report.sh` | 汇总测试用例通过/失败情况，生成 HTML 报告 |

## 2. 什么时候用
- 需要查看测试对代码的覆盖程度时，用 `gtest_coverage.sh`。
- 需要查看测试用例执行结果（通过率、失败明细、耗时）时，用 `gtest_report.sh`。

## 3. 怎么用

**覆盖率报告**（仅 Default/Ubuntu 平台）

```bash
# 一键运行测试并生成 HTML 报告
./gtest_coverage.sh --run-tests --strict-html
```

**测试结果报告**

```bash
# Ubuntu：一键运行用例并生成 HTML
./gtest_report.sh --run

# 其他平台：将板端导出的 xml 转为 HTML
./gtest_report.sh --xml test_internal.xml --xml test_external.xml
```

**报告输出**

| 类型 | 路径 |
|------|------|
| 覆盖率 HTML | `Release/Default/Coverage/html/index.html` |
| 测试结果 HTML | `Release/Default/TestReport/html/index.html` |

**依赖**：覆盖率报告需 `lcov`/`genhtml`；测试结果报告无需额外依赖（Python 内置库）。
