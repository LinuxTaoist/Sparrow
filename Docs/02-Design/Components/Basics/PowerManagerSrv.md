# PowerManagerSrv

## 1. 这是做什么的
PowerManagerSrv 是 Sparrow 的电源状态管理服务，负责电源状态切换和电源事件分发。

当前实现主要包括：
- 两级状态机管理（一级状态 + 二级过渡状态）。
- 启动、待机、睡眠事件按优先级分发。
- 预待机请求与模块应答收集（ALLOW/REFUSE/DELAY）。
- 调用硬件抽象层接口切换电源模式。

## 2. 什么时候会用到
当系统需要执行开机流程、进入待机/睡眠，或业务模块需要监听电源事件时，就会用到它。

## 3. 怎么用
业务模块通常通过 PowerManagerInterface 触发 PowerOn/PowerOff，或注册回调接收电源事件，不直接操作状态机细节。

接口使用文档请看：
- [PowerManagerInterface](../../../05-Reference/PublicAPIs/PowerManagerInterface.md)

相关实现入口：
- [PowerManagerInterface.h](../../../../PublicAPIs/PowerManagerInterface.h)
- [PowerManagerInterface.cpp](../../../../PublicAPIs/PowerManagerInterface.cpp)
- [main_power.cpp](../../../../Components/Basics/PowerManagerSrv/main_power.cpp)
- [PowerManager.h](../../../../Components/Basics/PowerManagerSrv/PowerManager.h)
- [PowerManager.cpp](../../../../Components/Basics/PowerManagerSrv/PowerManager.cpp)
- [PowerManagerHub.h](../../../../Components/Basics/PowerManagerSrv/PowerManagerHub.h)

## 4. 要注意什么
- 消息处理由状态表 `mStateTable` 驱动，同一消息在不同状态下会走不同响应函数。
- 预待机应答单次超时为 2000ms，总超时为 6000ms；总超时后会继续待机流程。
- 待机事件按优先级通过轮询定时器分发，分发完成后才真正进入 standby。
- 进入 standby 后会启动延时（5000ms）再进入 sleep。
- 某模块返回 `PRE_STANDBY_ACK_REFUSE` 会终止本次待机过渡并恢复到普通 active 二级状态。
- 对外事件通过 `POST_AEVENT` 上报，硬件状态切换通过 `HwInterfaceHub::GetPower()` 执行。