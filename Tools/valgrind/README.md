# valgrind 内存泄漏检测

## 1. 这是做什么的
用 valgrind 的 memcheck 工具启动 Sparrow 各服务，运行期间检测内存泄漏、越界访问、未初始化变量等内存问题，每个服务输出独立报告。

## 2. 什么时候用
- 怀疑服务存在内存泄漏、内存越界等疑难问题时。
- 版本发布前做内存质量检查时。

## 3. 怎么用

**依赖**：需先安装 valgrind

```bash
sudo apt install valgrind
```

**启动服务（带检测）**

```bash
./start_valgrind.sh [--bin-path <dir>] [--report-path <dir>]
```

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `--bin-path` | 当前路径 | 服务可执行文件目录 |
| `--report-path` | 当前路径下 `valgrind_logs` | 报告输出目录 |

**停止服务**

```bash
./stop_valgrind.sh [--report-path <dir>]
```

**查看结果**

每个服务的报告位于 `valgrind_logs/valgrind_<服务名>.log`，重点看两处：

- `LEAK SUMMARY`：`definitely lost` 大于 0 表示存在确定的内存泄漏。
- `ERROR SUMMARY`：大于 0 表示存在内存访问错误。

快速汇总所有服务的泄漏情况：

```bash
grep -H "definitely lost\|ERROR SUMMARY" valgrind_logs/valgrind_*.log
```
