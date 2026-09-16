# SparrowSrv

## 1. 这是做什么的
SparrowSrv 是 Sparrow 基础系统主服务，负责系统级初始化和基础模块装配。

当前实现主要包括：
- 初始化系统环境并输出版本信息文件。
- 初始化系统定时器与定时器管理器。
- 初始化 TimeManager 时间同步模块。
- 初始化 PluginManager 并完成后续基础准备。

## 2. 什么时候会用到
当 Sparrow 基础框架启动时就会用到它，它是核心基础服务之一。

## 3. 怎么用
SparrowSrv 主要承担系统装配职责，业务模块通常不直接调用 SparrowSrv，而是依赖它初始化后的公共能力。

相关实现入口：
- [main_sparrow.cpp](../../../../Components/Basics/SparrowSrv/main_sparrow.cpp)
- [SprSystem.h](../../../../Components/Basics/SparrowSrv/SprSystem.h)
- [SprSystem.cpp](../../../../Components/Basics/SparrowSrv/SprSystem.cpp)
- [PluginManager.h](../../../../Components/Basics/SparrowSrv/PluginManager.h)
- [PluginManager.cpp](../../../../Components/Basics/SparrowSrv/PluginManager.cpp)
- [SprSystemTimer.h](../../../../Components/Basics/SparrowSrv/SprSystemTimer.h)
- [SprTimerManager.h](../../../../Components/Basics/SparrowSrv/SprTimerManager.h)

## 4. 要注意什么
- 初始化顺序固定为：环境信息 -> 系统定时器 -> TimerManager -> TimeManager -> 其他模块准备。
- 版本信息会写入 `DEFAULT_DEBUG_ROOT_DIR/sparrow_version`，用于运行态排障。
- `SprProcPrepare::Init(SRV_NAME_SPARROW)` 在后置阶段执行，用于进程运行环境准备。
- 收到 `MAIN_EXIT_SIGNUM` 会退出事件循环；收到致命信号会打印回溯并退出。
- TimeManager 是由 SparrowSrv 内部初始化的基础模块，不是单独独立守护进程。