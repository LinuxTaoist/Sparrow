# ServiceManagerSrv

## 1. 这是做什么的
ServiceManagerSrv 是 Sparrow 的服务管理进程，用于：

- 按配置启动多个基础服务。
- 监控服务进程和心跳状态。
- 服务异常时自动重启。
- 系统优雅退出所有子服务。

## 2. 什么时候会用到
系统启动 Sparrow 服务栈时启动 ServiceManagerSrv 即可。

## 3. 怎么用
### 3.1 编辑配置文件

编辑 [init.conf](../../../../Configs/General/init.conf)，每行配置一个服务：

```text
logmanagersrv       [d]
bindermanagersrv    [d]
mediatorsrv         [d] [argv: --heartbeat=on]
propertiessrv       [d] [argv: --heartbeat=on]
configmanagersrv    [d] [argv: --heartbeat=on]
powermanagersrv     [d] [argv: --heartbeat=on]
sparrowsrv          [d] [argv: --heartbeat=on]
statusmonitorsrv        [argv: --heartbeat=on]
```

- 第一个字段是服务可执行文件路径。
- `[d]` 表示该服务参与依赖启动节奏控制。
- `[argv: ...]` 用于传递服务启动参数。
- `[argv: --heartbeat=on]` 开启心跳监控。
- 支持空行和 `#` 注释。

### 3.2 启动服务管理进程

直接启动 ServiceManagerSrv：

```text
./servicemanagersrv
```

ServiceManagerSrv 会自动读取 `init.conf`，按配置顺序启动服务并进入守护循环。

通过`killall -10  ServiceManagerSrv`停止 ServiceManagerSrv 后，它会按逆序优雅停止已启动的服务。

相关实现入口：
- [main_service.cpp](../../../../Components/Basics/ServiceManagerSrv/main_service.cpp)
- [ServiceManager.h](../../../../Components/Basics/ServiceManagerSrv/ServiceManager.h)
- [ServiceManager.cpp](../../../../Components/Basics/ServiceManagerSrv/ServiceManager.cpp)

## 4. 要注意什么
- `init.conf` 中的服务顺序就是启动顺序，当前不会根据依赖关系重新排序。
- 配置中标记依赖时，相邻服务拉起会插入 100ms 间隔。
- 服务启动前会校验路径存在、类型为普通文件且具有执行权限。
- 服务进程退出或心跳超时都会自动重启。
- 心跳通道默认是 `/tmp/heartbeat.sock`，默认超时时间为 10 秒。
- 重启前会重置心跳计时，避免同一次故障触发重复重启。
- 服务停止时先发送 `MAIN_EXIT_SIGNUM`，最多等待 5 秒，超时后发送 `SIGKILL`。
- 配置路径优先使用环境变量 `SPR_ROOT_PATH`，否则回退到可执行文件相对路径。