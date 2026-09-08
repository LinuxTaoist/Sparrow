# SprMediatorInterface

## 1. 这是做什么的
SprMediatorInterface 是 Sparrow 提供的中介服务查询接口。

它主要做两件事：
- 获取系统内所有模块消息队列状态。
- 根据信号 ID 查询对应信号名称。

## 2. 什么时候会用到
当需要查看模块通信状态、定位消息链路问题或将信号 ID 转成可读名称时会用到。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `SprMediatorInterface.h` | 中介查询接口 |
| 库文件 | `libmediatorapi.so` | Mediator 对外接口库 |

**典型流程**

1. 获取接口实例
	- 调用 `SprMediatorInterface::GetInstance()` 获取单例对象。
	- 初始化阶段会自动连接 `SRV_NAME_MEDIATOR`。

2. 查询消息队列状态
	- 调用 `GetAllMQStatus(mqAttrVec)` 获取所有模块 MQ 详情。

3. 查询信号名称
	- 调用 `GetSigalName(sig)` 把信号 ID 转成字符串名称。

**简单示例**

```cpp
#include "SprMediatorInterface.h"

SprMediatorInterface* med = SprMediatorInterface::GetInstance();
if (med == nullptr) {
    return -1;
}

std::vector<SMQueueDetails> allMqs;
med->GetAllMQStatus(allMqs);

std::string sigName = med->GetSigalName(1001);
```

**参考代码**
- 接口定义: [SprMediatorInterface.h](../../../PrivateAPIs/SprMediatorInterface.h)
- 接口实现: [SprMediatorInterface.cpp](../../../PrivateAPIs/SprMediatorInterface.cpp)

## 4. 要注意什么
- 初始化 Binder 失败会导致接口不可用，后续查询会直接失败或返回空字符串，参考 [SprMediatorInterface.cpp:48](../../../PrivateAPIs/SprMediatorInterface.cpp#L48) [SprMediatorInterface.cpp:49](../../../PrivateAPIs/SprMediatorInterface.cpp#L49) [SprMediatorInterface.cpp:71](../../../PrivateAPIs/SprMediatorInterface.cpp#L71)
- `GetAllMQStatus` 使用 `PROXY_CMD_GET_ALL_MQ_ATTRS` 命令，并在 ret=0 时才读取向量数据，参考 [SprMediatorInterface.cpp:77](../../../PrivateAPIs/SprMediatorInterface.cpp#L77) [SprMediatorInterface.cpp:84](../../../PrivateAPIs/SprMediatorInterface.cpp#L84)
- `GetSigalName` 失败返回空字符串 `""`，调用方需判空，参考 [SprMediatorInterface.cpp:99](../../../PrivateAPIs/SprMediatorInterface.cpp#L99)
- 所有请求通过 `ProcLockGuard(gPMutex, gTMutex)` 串行保护，参考 [SprMediatorInterface.cpp:76](../../../PrivateAPIs/SprMediatorInterface.cpp#L76)
