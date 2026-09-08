# SprObserverWithMQueue

## 1. 这是做什么的
`SprObserverWithMQueue` 是 Sparrow 中基于消息队列的观察者基类，适合需要在进程内进行模块间消息通信的组件。

它的核心职责包括：
- 作为消息队列观察者，接收来自 mediator 或其他模块的消息；
- 通过 epoll 事件回调驱动消息处理；
- 在派发消息时调用子类实现的 `ProcessMsg()`，完成业务逻辑处理；
- 维护模块注册/注销和状态同步逻辑，方便业务组件快速接入。

从设计上看，它是“消息驱动型组件”的基础抽象，适用于轻量处理、低延迟、事件驱动的业务模型。

## 2. 什么时候会用到
适合以下场景：
- 模块之间需要异步消息通信；
- 组件要监听消息并做状态切换、事件回调、指令处理；
- 业务处理相对短小，能直接在 epoll 线程中完成；
- 需要把模块注册到 mediator，统一管理运行时状态。

如果业务处理较慢、耗时较长，建议考虑使用 `SprObserverWithMQueueThread`。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `SprObserverWithMQueue.h` | 观察者基类定义 |
| 相关组件 | `SprMsg.h`、`PMsgQueue.h`、`SprObserver.h` | 消息结构与基础观察者接口 |
| 运行时 | mediator / message queue | 负责消息注册和收发 |

**典型流程**

1. 派生一个业务类，继承 `SprObserverWithMQueue`。
2. 实现纯虚函数 `ProcessMsg(const SprMsg& msg)`。
3. 在组件初始化时调用父类初始化逻辑，完成 mediator 注册和消息队列绑定。
4. 通过 `SendMsg()` 发送消息，或由外部消息触发 `EpollEvent()`。

**简要示例**

```cpp
class MyObserver : public SprObserverWithMQueue {
public:
    MyObserver(ModuleIDType id, const std::string& name)
        : SprObserverWithMQueue(id, name) {}

protected:
    int32_t ProcessMsg(const SprMsg& msg) override {
        // 处理业务消息
        return 0;
    }
};
```

**参考代码**
- 接口定义: [../../../Core/SprObserverWithMQueue.h](../../../Core/SprObserverWithMQueue.h)
- 实现代码: [../../../Core/SprObserverWithMQueue.cpp](../../../Core/SprObserverWithMQueue.cpp)

## 4. 和 `SprObserverWithMQueueThread` 的差异
最简要的差异是：

- `SprObserverWithMQueue`：在 epoll 事件回调中直接处理收到的消息；
- `SprObserverWithMQueueThread`：将收到的消息先入队，再由独立线程按 FIFO 顺序处理。

也就是说：
- 前者更轻量，适合消息处理快、逻辑简单的组件；
- 后者更适合处理耗时业务，避免长任务堵塞共享 epoll 线程。

## 5. 要注意什么
- `ProcessMsg()` 是核心扩展点，业务逻辑应尽量保持短小、可控。
- 如果消息处理可能耗时较长，容易拖慢整个事件循环，需要考虑线程版实现。
- 组件退出前应保证消息注册/注销和资源清理完整，避免残留信号量或消息队列状态。
