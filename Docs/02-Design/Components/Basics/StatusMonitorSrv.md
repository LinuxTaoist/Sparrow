# StatusMonitorSrv

## 1. 这是做什么的
StatusMonitorSrv 是 Sparrow 的状态事件监控服务，负责接收并管理各模块上报的错误/状态事件。

当前实现主要包括：
- 接收 `SIG_ID_MONITOR_STATUS_EVENT` 事件并按模块归档。
- 按模块、级别、错误码、文本过滤查询。
- 支持按过滤条件删除历史事件。
- 提供调试命令用于事件增删查和时间显示。

## 2. 什么时候会用到
当模块需要上报运行异常，或排障时需要查询历史状态事件时，就会用到它。

## 3. 怎么用
业务模块通常通过内部事件上报链路发送状态事件，不直接依赖 StatusMonitorSrv 的内部容器结构。

相关实现入口：
- [main_monitor.cpp](../../../../Components/Basics/StatusMonitorSrv/main_monitor.cpp)
- [StatusMonitorManager.h](../../../../Components/Basics/StatusMonitorSrv/StatusMonitorManager.h)
- [StatusMonitorManager.cpp](../../../../Components/Basics/StatusMonitorSrv/StatusMonitorManager.cpp)

## 4. 要注意什么
- 每个 moduleID 默认最多保留 100 条事件，超限后会删除最早记录。
- 事件按 `moduleID -> list<StatusEvent>` 存储，Dump 时按逆序输出，最新事件优先显示。
- 过滤条件支持 moduleID、错误级别、错误码和文本精确匹配。
- 事件时间基于 `time(nullptr)` 记录，展示时转换为本地时间字符串。
- 该服务提供较多 DebugNode 命令，适合联调期快速排查。