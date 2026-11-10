# SparrowSrv

## 1. 这是做什么的
SparrowSrv 是 Sparrow 的业务插件宿主进程 `sparrowsrv`，负责运行环境信息输出和业务模块装配。

当前实现主要包括：
- 初始化系统环境并输出版本信息文件。
- 初始化 `PluginManager`，加载业务插件并调用插件入口。
- 提供插件状态调试与可选的目录热插拔监控。
- 退出时卸载插件并调用插件退出入口。

## 2. 什么时候会用到
需要通过动态库加载业务组件时使用。业务插件可以依赖已启动的消息、时间、属性、配置和电源等基础服务，因此宿主进程安排在这些服务之后启动。

## 3. 怎么用
业务插件提供 `PluginEntry` 和 `PluginExit` 入口，由 `PluginManager` 负责加载与卸载。宿主扫描运行目录对应的 `Lib` 目录，加载名称以 `libplugin` 开头的库。

全局消息定时器与系统校时由 [TimeManagerSrv](TimeManagerSrv.md) 提供，业务模块通过公共消息接口使用这些能力，不直接调用宿主的初始化逻辑。

相关实现入口：
- [main_sparrow.cpp](../../../../Components/Basics/SparrowSrv/main_sparrow.cpp)
- [SprSystem.h](../../../../Components/Basics/SparrowSrv/SprSystem.h)
- [SprSystem.cpp](../../../../Components/Basics/SparrowSrv/SprSystem.cpp)
- [PluginManager.h](../../../../Components/Basics/SparrowSrv/PluginManager.h)
- [PluginManager.cpp](../../../../Components/Basics/SparrowSrv/PluginManager.cpp)

## 4. 要注意什么
- 初始化顺序为：环境与版本信息 -> 业务插件装配 -> 进程调试节点与心跳 -> 事件循环。
- 版本信息会写入 `DEFAULT_DEBUG_ROOT_DIR/sparrow_version`，用于运行态排障。
- 调试管道为 `/tmp/sparrowsrv`，用于访问插件管理和当前进程的调试命令。
- 插件热插拔默认关闭，运行时由 `PluginManager` 的调试命令控制。
- 收到 `MAIN_EXIT_SIGNUM` 会退出事件循环；收到致命信号会打印回溯并退出。