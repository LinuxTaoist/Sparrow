# cppcheck 静态代码扫描

## 1. 这是做什么的
基于 cppcheck 对工程源码做静态分析，在编译之外提前发现潜在缺陷，输出 XML 结果报告。

覆盖的检查项：内存泄漏、越界访问、未初始化变量、空指针解引用、逻辑错误等（`--enable=all`）。

## 2. 什么时候用
- 提交代码前或定期做代码质量检查时。
- 排查隐蔽缺陷（如资源未释放、边界条件遗漏）时，作为编译器和单测之外的补充。

## 3. 怎么用

**依赖**：需先安装 cppcheck

```bash
sudo apt install cppcheck
```

**运行**

```bash
cd Tools/cppcheck
./RunCppcheck.sh
```

**结果**

- 报告输出到当前目录，命名 `StaticScan_YYYYMMDD_NN.xml`（同日多次运行自动递增序号）。
- 脚本先做 `--check-config` 检测缺失头文件，若发现会提示补充 `EXTRA_INCLUDES`；未缺失则直接扫描。

**忽略名单**

第三方 cJSON（`Util/cJSON.*`、`UtilModules/CodecX/cJSON.*`）默认排除，如需调整在 `CHECK_IGNORE` 数组中修改。
