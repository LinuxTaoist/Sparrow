# ServiceManagerSrv

## 1. 这是做什么的
ServiceManagerSrv 是 Sparrow 的服务拉起与守护进程，负责按配置启动基础服务并监控其存活状态。

当前实现主要包括：
- 读取 `init.conf` 启动服务列表。
- fork/exec 拉起服务可执行文件。
- 监听子进程退出并执行自动重启。
- 系统退出时按逆序优雅停止服务，必要时强杀兜底。

## 2. 什么时候会用到
当系统启动 Sparrow 服务栈，或者某个基础服务异常退出需要自动拉起时，就会用到它。

## 3. 怎么用
ServiceManagerSrv 作为系统基础守护进程运行，业务模块一般不直接调用其内部接口。

相关实现入口：
- [main_service.cpp](../../../../Components/Basics/ServiceManagerSrv/main_service.cpp)
- [ServiceManager.h](../../../../Components/Basics/ServiceManagerSrv/ServiceManager.h)
- [ServiceManager.cpp](../../../../Components/Basics/ServiceManagerSrv/ServiceManager.cpp)
- [init.conf](../../../../Configs/General/init.conf)

## 4. 要注意什么
- `init.conf` 支持注释和空行，行尾 `[d]` 表示依赖启动节奏控制。
- 配置中标记依赖时，相邻服务拉起会插入 100ms 间隔。
- 子进程退出后会标记为 dead 并尝试重启，重启前固定延迟 500ms。
- 主循环每秒轮询一次退出状态，异常退出检测存在最多 1s 延迟。
- 停止流程会先发 `MAIN_EXIT_SIGNUM`，超时未退出再发 `SIGKILL`。
- 配置路径优先使用环境变量 `SPR_ROOT_PATH`，否则回退到可执行文件相对路径。