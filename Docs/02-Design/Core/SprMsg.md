# SprMsg

## 1. 这是做什么的
SprMsg 是 Sparrow 内部消息对象，负责在模块间传递统一消息结构。

它主要做两件事：
- 管理消息头字段（From、To、MsgId）和多种数据载荷（基础类型、字符串、向量、原始字节数据）。
- 提供二进制编码与解码能力，支持把消息对象和字节流互相转换。

## 2. 什么时候会用到
当模块需要通过中介层、消息队列或跨线程通道传递结构化消息时会用到，适用于事件通知、状态同步和命令分发场景。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `SprMsg.h` | 消息对象接口 |
| 库文件 | `libsprcore.so` | 包含 SprMsg 实现 |

**典型流程**

1. 构造消息并设置头字段
	- 创建 `SprMsg` 对象。
	- 设置 `From`、`To`、`MsgId`，明确消息来源、目标和业务语义。

2. 写入业务数据
	- 根据数据类型调用 `SetBoolValue`、`SetU32Value`、`SetString`、`SetU8Vec` 等接口。
	- 也可以通过 `SetDatas` 写入原始字节块。

3. 编码发送与解码读取
	- 发送前调用 `Encode` 生成字节流。
	- 接收端通过 `SprMsg(bytes)` 或 `Decode` 还原消息，再调用 `GetXXX` 读取数据。

**简单示例**

1. 发送端构造并编码

```cpp
SprMsg msg(MODULE_MEDIATOR, SIG_ID_PROXY_REGISTER_REQUEST);
msg.SetFrom(MODULE_PROXY);
msg.SetString("register:moduleA");
msg.SetU32Value(1001);

std::string bytes;
if (msg.Encode(bytes) != 0) {
    return -1;
}
```

2. 接收端解码并读取

```cpp
SprMsg recvMsg(bytes);
if (recvMsg.GetMsgId() == SIG_ID_PROXY_REGISTER_REQUEST) {
    std::string name = recvMsg.GetString();
    uint32_t moduleId = recvMsg.GetU32Value();
}
```

**参考代码**
- 接口定义: [SprMsg.h](../../../Core/SprMsg.h)
- 接口实现: [SprMsg.cpp](../../../Core/SprMsg.cpp)
- 使用示例: [SprMediator.cpp](../../../Components/Basics/MediatorSrv/SprMediator.cpp)
- 单元测试: [TestSprMsg.cc](../../../TestCase/Internal/Core/TestSprMsg.cc)

## 4. 要注意什么
- `DumpBrief` 的成员输出长度上限是 5，超出会用 `...` 截断，参考 [SprMsg.cpp:26](../../../Core/SprMsg.cpp#L26)

