# TimeManagerSrv

## 1. 这是做什么的
TimeManagerSrv 是 Sparrow 的基础时间服务，运行在独立进程 `timemanagersrv` 中，提供全局消息定时器和系统时间校准能力。

进程内模块分工：

| 模块 | 职责 |
| --- | --- |
| `SprSystemTimer` | 封装底层 timerfd，提供定时到期通知。 |
| `SprTimerManager` / `SprTimer` | 管理各模块的消息定时器，处理注册、注销、重复次数与超时消息投递。 |
| `TimeManager` | 按优先级请求时间源，根据偏差量选择平滑校时或跳变校时，并响应电源状态事件。 |
| `NtpSource` / `NtpProtocol` | 请求 NTP 时间、处理报文并计算时间偏差。 |

## 2. 什么时候会用到
- 模块需要延迟执行、周期调度或超时通知时，使用全局消息定时器。
- 系统需要通过 NTP 校准时钟，或周期性保持时钟精度时，使用时间同步能力。
- 电源管理等基础服务使用消息定时器，因此本服务应在电源服务和业务插件宿主之前启动。

## 3. 怎么用
### 3.1 启动服务

平台的 `BASIC_MODULES` 包含 `TimeManagerSrv`，启动配置中将 `timemanagersrv` 放在 `mediatorsrv` 之后、`powermanagersrv` 之前：

```text
mediatorsrv         [d] [argv: --heartbeat=on]
timemanagersrv      [d] [argv: --heartbeat=on]
```

进程按 `SprSystemTimer`、`SprTimerManager`、`TimeManager` 的顺序初始化，然后进入事件循环。定时器请求通过消息中介传输，不依赖 `sparrowsrv` 或业务插件加载。

启动配置优先使用当前平台的同名文件，缺失时使用 [通用配置](../../../../Configs/General/init.conf)。完整配置方式见 [ServiceManagerSrv](ServiceManagerSrv.md)。

### 3.2 使用消息定时器

模块通过 `SprObserver::RegisterTimer(delay, interval, msgId, repeatTimes)` 注册定时器，通过 `UnregisterTimer(msgId)` 注销。定时器服务使用 `MODULE_TIMERM` 接收请求，到期后向调用模块投递 `msgId`；`repeatTimes` 为 0 表示持续重复。

业务模块无需直接持有 `SprTimerManager` 或 `TimeManager` 对象。系统校时由 `TimeManager` 根据电源事件控制，NTP 请求提交到线程池执行。

### 3.3 调试入口

调试管道为 `/tmp/timemanagersrv`，常用命令：

```bash
echo help > /tmp/timemanagersrv
echo DumpTimers > /tmp/timemanagersrv
echo DumpDetails > /tmp/timemanagersrv
```

`DumpTimers` 查看全局消息定时器，`DumpDetails` 查看时间源、同步状态及校时计数。`StartSyncTime` 可触发一次同步流程，`StartPoll` / `StopPoll` 控制周期同步。

相关实现入口：
- [main_time.cpp](../../../../Components/Basics/TimeManagerSrv/main_time.cpp)
- [SprSystemTimer.h](../../../../Components/Basics/TimeManagerSrv/SprSystemTimer.h)
- [SprTimerManager.h](../../../../Components/Basics/TimeManagerSrv/SprTimerManager.h)
- [SprTimer.h](../../../../Components/Basics/TimeManagerSrv/SprTimer.h)
- [TimeManager.h](../../../../Components/Basics/TimeManagerSrv/TimeManager.h)
- [TimeManager.cpp](../../../../Components/Basics/TimeManagerSrv/TimeManager.cpp)
- [NtpSource.h](../../../../Components/Basics/TimeManagerSrv/NtpSource.h)
- [NtpSource.cpp](../../../../Components/Basics/TimeManagerSrv/NtpSource.cpp)
- [NtpProtocol.h](../../../../Components/Basics/TimeManagerSrv/NtpProtocol.h)
- [NtpProtocol.cpp](../../../../Components/Basics/TimeManagerSrv/NtpProtocol.cpp)

## 4. 要注意什么
- 消息定时器的最小重复间隔为 100ms；延迟为 0 表示尽快触发，不表示同步执行。
- 定时器使用单调时钟计时，系统校时调整的是 `CLOCK_REALTIME`；定时器服务可用与系统时间已校准是不同状态。
- 定时器服务启动不等待网络或 NTP 校时成功。NTP 需要可用的网络与 DNS，调整系统时钟需要相应权限。
- 默认同步超时是 4000ms，默认轮询周期是 600000ms。
- 默认内置 NTP 服务器包括 `ntp.tencent.com:123` 和 `ntp1.aliyun.com:123`。
- 偏差绝对值小于等于 50ms 不调整，50ms 到 10s 走平滑调整，大于 10s 走跳变调整。
- 收到 `SIG_ID_POWER_STARTUP_HIGH` 启动轮询并尽快触发首轮同步，收到 `SIG_ID_POWER_STANDBY_LOW` 停止周期同步；全局消息定时器不随校时轮询停止。
- `RequestGnssTime()` 当前返回失败，GNSS 通道仍需后续硬件能力接入。
- 消息定时器保存在进程内存中，服务重启后需要使用方重新注册，心跳拉起不等于定时任务已恢复。