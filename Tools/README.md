# xtools 开发辅助工具

## 1. 这是做什么的
Sparrow 的统一开发辅助入口，用一条命令封装了环境查看、编译、静态扫描、内存检测等常用操作。

## 2. 什么时候用
日常开发中需要快速执行环境检查、全量编译、静态扫描、valgrind 检测等操作时，无需记忆各脚本路径。

## 3. 怎么用

```bash
./xtools.sh <command>
```

| 命令 | 说明 |
|------|------|
| `env` | 查看当前环境（系统、编译器、cmake 版本） |
| `build-all` | 编译整个项目（含第三方库） |
| `build-3rd` | 编译依赖的第三方库 |
| `staticscan` | 执行 cppcheck 静态代码扫描 |
| `start-valgrind` | 启动 valgrind 内存检测 |
| `stop-valgrind` | 停止 valgrind |
| `new-platform <name>` | 创建新平台目录骨架 |
| `adb-push <platform> <path>` | 推送产物到设备 |
| `commit-template` | 配置 git 提交模板 |
| `sync-code` | 同步代码仓库 |
| `help` | 显示帮助信息 |

**示例**

```bash
# 查看环境
./xtools.sh env

# 全量编译
./xtools.sh build-all

# 静态扫描
./xtools.sh staticscan
```
