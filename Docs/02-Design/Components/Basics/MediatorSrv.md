# MediatorSrv

## 1. 这是做什么的
MediatorSrv 是 Sparrow 的消息中介服务，负责模块消息队列注册、注销和消息转发。

当前实现主要包括：
- 维护模块 ID 到消息队列对象的映射。
- 处理注册/注销请求并返回结果。
- 单播和广播消息分发。
- 提供消息队列状态查询能力。

## 2. 什么时候会用到
当模块需要跨进程发送 SprMsg，或者系统需要查看当前各模块消息队列状态时，就会用到它。

## 3. 怎么用
模块通常通过内部代理链路向 MediatorSrv 注册后参与消息通信，业务方一般不直接操作 MediatorSrv 实现。

接口使用文档请看：
- [SprMediatorInterface](../../PrivateAPIs/SprMediatorInterface.md)

相关实现入口：
- [SprMediatorInterface.h](../../../../PrivateAPIs/SprMediatorInterface.h)
- [SprMediatorInterface.cpp](../../../../PrivateAPIs/SprMediatorInterface.cpp)
- [main_mediator.cpp](../../../../Components/Basics/MediatorSrv/main_mediator.cpp)
- [SprMediator.h](../../../../Components/Basics/MediatorSrv/SprMediator.h)
- [SprMediator.cpp](../../../../Components/Basics/MediatorSrv/SprMediator.cpp)
- [SprMediatorHub.h](../../../../Components/Basics/MediatorSrv/SprMediatorHub.h)

## 4. 要注意什么
- MediatorSrv 内部会创建固定名称的内部消息队列（由 `MEDIATOR_MSG_QUEUE` 宏定义）。
- 模块注册消息需要提供模块 ID、队列名和 monitored 标记，注册成功后才可收发消息。
- 当消息 `to` 为 `MODULE_NONE` 时，MediatorSrv 会按广播策略分发给符合条件的模块。
- 注销模块后会移除其队列详情，并向其他模块广播组件退出消息。
- `GetAllMQStatus` 返回的是当前已登记队列的快照信息。