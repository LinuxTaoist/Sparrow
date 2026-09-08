# PowerManagerInterface

## 1. 这是做什么的
PowerManagerInterface 是 Sparrow 对外提供的电源管理控制接口。

它主要做两件事：
- 通过 Binder 客户端向 PowerManager 服务发送开关机控制命令。
- 提供事件回调注册能力，接收异步电源事件通知。

## 2. 什么时候会用到
当业务模块需要主动触发上电/下电，或监听电源状态变化事件时会用到。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `PowerManagerInterface.h` | 电源管理接口 |
| 库文件 | `libpowermapi.so` | PowerManager 对外接口库 |

**典型流程**

1. 获取接口实例
	- 调用 `PowerManagerInterface::GetInstance()` 获取单例对象。
	- 初始化阶段会自动连接 `SRV_NAME_POWER_MANAGER` 服务。

2. 发送电源控制命令
	- 调用 `PowerOn()` 触发上电。
	- 调用 `PowerOff()` 触发下电。

3. 注册和注销异步回调
	- 调用 `RegisterCallback(callback)` 注册回调并启动异步事件读取。
	- 退出前调用 `UnRegisterCallback()` 注销回调。

**简单示例**

1. 电源控制

```cpp
#include "PowerManagerInterface.h"

PowerManagerInterface* pm = PowerManagerInterface::GetInstance();
if (pm == nullptr) {
    return -1;
}

if (pm->PowerOn() != 0) {
    return -1;
}

// ...

pm->PowerOff();
```

2. 注册回调

```cpp
static void OnPowerEvent(int32_t eventID, void* data, int32_t size)
{
    (void)data;
    (void)size;
    // 处理电源事件
}

pm->RegisterCallback(OnPowerEvent);
// 退出前
pm->UnRegisterCallback();
```

**参考代码**
- 接口定义: [PowerManagerInterface.h](../../../PublicAPIs/PowerManagerInterface.h)
- 接口实现: [PowerManagerInterface.cpp](../../../PublicAPIs/PowerManagerInterface.cpp)

## 4. 要注意什么
- 构造阶段若 `InitializeClientBinder(SRV_NAME_POWER_MANAGER, ...)` 失败，接口会被置为不可用，后续方法直接返回 -1，参考 [PowerManagerInterface.cpp:51](../../../PublicAPIs/PowerManagerInterface.cpp#L51)
- `RegisterCallback` 成功后会调用 `AsyncEvent::AsReader(SRV_NAME_POWER_MANAGER)` 启动异步读取并注册回调函数，参考 [PowerManagerInterface.cpp:128](../../../PublicAPIs/PowerManagerInterface.cpp#L128)
- 所有同步请求通过 `ProcLockGuard(gPMutex, gTMutex)` 串行保护，避免并发请求冲突，参考 [PowerManagerInterface.cpp:80](../../../PublicAPIs/PowerManagerInterface.cpp#L80)

