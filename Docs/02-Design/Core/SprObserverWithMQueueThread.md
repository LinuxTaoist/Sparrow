# SprObserverWithMQueueThread

## 1. 这是做什么的
`SprObserverWithMQueueThread` 是 `SprObserverWithMQueue` 的线程版封装，用于把消息处理从 epoll 事件循环中剥离出来。

它的核心思路是：
- 收到消息后，不直接在 epoll 回调中执行业务处理；
- 转而将消息压入内部 FIFO 队列；
- 由专用线程按顺序消费并回调 `DispatchSprMsg()`；
- 让耗时业务处理不阻塞共享事件循环，提高组件稳定性和吞吐。

从设计上，它保留了和 `SprObserverWithMQueue` 相同的消息接口，但增加了“异步处理线程”的能力。

## 2. 什么时候会用到
适合以下场景：
- 消息处理逻辑中含有较慢的 IO、日志、计算或状态更新；
- 同一组件收到消息很频繁，但业务处理不适合在 epoll 线程中阻塞；
- 希望保留 FIFO 顺序处理，同时避免事件循环被长任务拖住。

典型例子：轮询处理、状态汇总、网络协议解包后的业务处理等。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `SprObserverWithMQueueThread.h` | 线程版观察者定义 |
| 基类 | `SprObserverWithMQueue` | 仍然复用消息队列和 mediator 机制 |
| 运行时 | `std::thread` / `std::mutex` / `std::condition_variable` | 内部线程与队列同步 |

**典型流程**

1. 派生一个业务类，继承 `SprObserverWithMQueueThread`。
2. 实现 `ProcessMsg(const SprMsg& msg)`，和普通消息观察者方式一致。
3. 组件初始化后，线程会在首条消息到达时自动启动，并保持运行直到对象销毁。
4. 处理线程会按 FIFO 顺序处理消息，不会阻塞 epoll 事件循环。

**简要示例**

```cpp
class MyThreadObserver : public SprObserverWithMQueueThread {
public:
    MyThreadObserver(ModuleIDType id, const std::string& name)
        : SprObserverWithMQueueThread(id, name) {}

protected:
    int32_t ProcessMsg(const SprMsg& msg) override {
        // 在线程中处理消息
        return 0;
    }
};
```

**参考代码**
- 接口定义: [../../../Core/SprObserverWithMQueueThread.h](../../../Core/SprObserverWithMQueueThread.h)
- 实现代码: [../../../Core/SprObserverWithMQueueThread.cpp](../../../Core/SprObserverWithMQueueThread.cpp)
- 相关测试: [../../../TestCase/Internal/Core/TestSprObserverWithMQueueThread.cc](../../../TestCase/Internal/Core/TestSprObserverWithMQueueThread.cc)

## 4. 和 `SprObserverWithMQueue` 的差异
最核心的差异如下：

- `SprObserverWithMQueue`：消息在 epoll 回调中被直接调用 `DispatchSprMsg()` 处理；
- `SprObserverWithMQueueThread`：收到消息后先入 `mMsgQueue`，然后由后台线程统一消费。

换句话说：
- `SprObserverWithMQueue` 更适合轻量、低延迟处理；
- `SprObserverWithMQueueThread` 更适合慢处理、长任务、流量峰值场景；
- 两者的接口几乎一致，业务侧切换时主要是“基类替换”而已。

## 5. 要注意什么
- 线程版会增加一个内部队列和线程，需注意队列长度和背压策略，避免消息堆积过多。
- `ProcessMsg()` 仍需保证共享状态的线程安全，避免与其他线程同时访问同一对象时产生竞争和状态错乱。
