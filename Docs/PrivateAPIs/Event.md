# Event

## 1. 这是做什么的
PrivateAPIs/Event 提供 Sparrow 的异步事件通道能力，核心实现为 `AsyncEvent`。

它主要做两件事：
- 通过共享 `Parcel` 通道发送异步事件（事件 ID + 数据负载）。
- 在接收侧启动回调线程，持续读取事件并分发到业务回调函数。

## 2. 什么时候会用到
当模块需要跨进程或跨模块异步通知事件，并以回调方式接收处理时会用到。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `AsyncEvent.h` | 异步事件接口 |
| 库文件 | `libsprbinder.so` | Event 与 Binder 聚合库 |

**典型流程**

1. 发送侧初始化
	- 调用 `AsWriter(name)` 创建事件写通道（内部名称会自动追加 `_event` 后缀）。

2. 接收侧初始化并注册回调
	- 调用 `AsReader(name)` 创建事件读通道。
	- 调用 `RegisterEventCallback(callback)` 启动读取线程并注册回调。

3. 发送和注销
	- 发送侧调用 `EventNotify(eventID, data, size)` 投递事件。
	- 退出前调用 `UnregisterEventCallback()` 关闭回调线程。

**简单示例**

```cpp
#include "AsyncEvent.h"

static void OnEvent(int32_t eventID, void* data, int32_t size)
{
    (void)data;
    (void)size;
}

AsyncEvent::GetInstance()->AsReader("power_manager");
AsyncEvent::GetInstance()->RegisterEventCallback(OnEvent);

AsyncEvent::GetInstance()->AsWriter("power_manager");
AsyncEvent::GetInstance()->EventNotify(1001, nullptr, 0);
```

**参考代码**
- 接口定义: [AsyncEvent.h](../../PrivateAPIs/Event/AsyncEvent.h)
- 接口实现: [AsyncEvent.cpp](../../PrivateAPIs/Event/AsyncEvent.cpp)
- 调用示例: [PowerManagerInterface.cpp](../../PublicAPIs/PowerManagerInterface.cpp)

## 4. 要注意什么
- 事件通道固定使用 `KEY_EVENT_NOTIFY = 99999`，且通道名会追加 `_event` 后缀，参考 [AsyncEvent.cpp:25](../../PrivateAPIs/Event/AsyncEvent.cpp#L25)
- `RegisterEventCallback` 仅允许注册一次：回调为空或线程已启动时返回 -1，参考 [AsyncEvent.cpp:80](../../PrivateAPIs/Event/AsyncEvent.cpp#L80)
- 回调线程内部是阻塞 `Wait()` 循环，未注销会持续运行，参考 [AsyncEvent.cpp:89](../../PrivateAPIs/Event/AsyncEvent.cpp#L89)
- `UnregisterEventCallback()` 直接 `join` 线程，调用前应确保线程已启动，参考 [AsyncEvent.cpp:70](../../PrivateAPIs/Event/AsyncEvent.cpp#L70)
