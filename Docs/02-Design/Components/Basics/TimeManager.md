# TimeManager

## 1. 这是做什么的
TimeManager 是 Sparrow 的系统时间同步模块，负责按优先级请求时间源并校准系统时间。

当前实现主要包括：
- 管理时间源优先级（当前包含 NTP/GNSS 通道）。
- 定时触发时间同步和轮询同步。
- 根据偏差量选择平滑校时或跳变校时。
- 响应电源状态事件，在启动和待机间切换同步策略。

## 2. 什么时候会用到
当系统启动后需要同步系统时钟，或需要周期性保持系统时钟精度时，就会用到它。

## 3. 怎么用
TimeManager 作为内部基础模块由 SparrowSrv 初始化，业务模块一般不直接持有它。

相关实现入口：
- [SprSystem.cpp](../../../../Components/Basics/SparrowSrv/SprSystem.cpp)
- [TimeManager.h](../../../../Components/Basics/TimeManager/TimeManager.h)
- [TimeManager.cpp](../../../../Components/Basics/TimeManager/TimeManager.cpp)
- [NtpSource.h](../../../../Components/Basics/TimeManager/NtpSource.h)
- [NtpSource.cpp](../../../../Components/Basics/TimeManager/NtpSource.cpp)
- [NtpProtocol.h](../../../../Components/Basics/TimeManager/NtpProtocol.h)
- [NtpProtocol.cpp](../../../../Components/Basics/TimeManager/NtpProtocol.cpp)

## 4. 要注意什么
- 默认同步超时是 4000ms，默认轮询周期是 600000ms。
- 默认内置 NTP 服务器包括 `ntp.tencent.com:123` 和 `ntp1.aliyun.com:123`。
- 偏差绝对值小于等于 50ms 不调整，50ms 到 10s 走平滑调整，大于 10s 走跳变调整。
- NTP 请求会提交到线程池执行，避免阻塞主消息处理流程。
- 收到 `SIG_ID_POWER_STARTUP_HIGH` 会启动轮询，同步到待机阶段收到 `SIG_ID_POWER_STANDBY_LOW` 会停止轮询。
- `RequestGnssTime()` 当前返回失败，GNSS 通道仍需后续硬件能力接入。