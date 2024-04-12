[TOC]
## 1. Q: 为什么即使同一个进程每个组件都要持有一个mq，而不是同一个进程组件共享一个mq？
**A:** 方便消息快速转发。在使用epoll时，会将mq和对应的组件SprObserver绑定，接收到mq消息时，会立刻丢到对应绑定的SprObserver组件去处理。如果共用一个mq，在接收到消息时，首先需要先解析是哪个组件，然后再派发消息，流程更多，影响传输效率。

## 2. Q: SprSigId.h中ESprSigId和CoreTypeDefs.h的MsgID区别？
**A:** ESprSigId用于内部组件之间消息的传递，异步方式；CoreTypeDefs.h中的MsgID用于RPC接口传递，可同步也可异步。两者不可统一，便于实现RPC接口同步或异步的方式。
