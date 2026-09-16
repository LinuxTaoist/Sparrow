# DebugSrv

## 1. 这是做什么的
DebugSrv 是 Sparrow 的调试能力服务，负责处理调试类消息并提供远程调试入口。

当前实现主要包括：
- 远程 Shell 开关控制（EnableRemoteShell/DisableRemoteShell）。
- 调试定时器测试消息转发（转发到 TimerManager）。
- 调试广播消息分发（NotifyAllObserver）。
- 调试命令注册（start1sTimer/stop1sTimer/sleep）。

## 2. 什么时候会用到
当系统需要远程调试、定时器联调或跨模块广播测试消息时，就会用到它。

## 3. 怎么用
在使用调试功能时，业务模块通常通过 Debug 接口发送调试请求，不直接操作 DebugSrv 进程内部逻辑。

接口使用文档请看：
- [DebugInterface](../../PrivateAPIs/DebugInterface.md)

相关实现入口：
- [DebugInterface.h](../../../../PrivateAPIs/DebugInterface.h)
- [DebugInterface.cpp](../../../../PrivateAPIs/DebugInterface.cpp)
- [main_debug.cpp](../../../../Components/Basics/DebugSrv/main_debug.cpp)
- [DebugModule.h](../../../../Components/Basics/DebugSrv/DebugModule.h)
- [DebugModule.cpp](../../../../Components/Basics/DebugSrv/DebugModule.cpp)
- [RemoteShell.h](../../../../Components/Basics/DebugSrv/RemoteShell.h)

## 4. 要注意什么
- `sleep` 调试命令会在 DebugSrv 线程中执行阻塞睡眠，不适合在实时路径长期使用。
- `SIG_ID_TIMER_ADD_CUSTOM_TIMER` 和 `SIG_ID_TIMER_DEL_CUSTOM_TIMER` 会被转发到 `MODULE_TIMERM`。
- `SIG_ID_DEBUG_NOTIFY_ALL` 会走广播路径，除消息源外的已注册模块都可能收到消息。
- RemoteShell 通过模块内部 `RemoteShell` 实例启停，开关由调试消息触发。
- 进程收到 `MAIN_EXIT_SIGNUM` 时会退出事件循环。