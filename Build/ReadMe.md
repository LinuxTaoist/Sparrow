# 编译说明

`Build` 目录提供常用编译入口。目录中的 Shell 脚本是指向对应平台构建脚本的软链接，用于避免每次进入较深的 `Platform/<平台>/Build` 路径。当前两个入口指向 `Default` 平台。

## 脚本区别

- `build_default.sh`：Debug 增量编译，适合日常开发和调试。无参数时编译全部目标；带参数时只编译指定目标及其依赖。
- `rebuild_default.sh`：清理已有产物和第三方构建缓存后执行 Release 全量编译，适合发布版本。

模块编译参数必须是有效的 CMake target，支持同时指定多个 target。

## 示例

```bash
cd Build

# Debug 全量增量编译
./build_default.sh

# Debug 单模块编译
./build_default.sh LibSprCore

# Debug 多模块编译
./build_default.sh LibSprCore LibSprHal

# Release 清理后全量编译（发版）
./rebuild_default.sh
```
