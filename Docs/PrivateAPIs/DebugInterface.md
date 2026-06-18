# DebugInterface

## 1. 这是做什么的
DebugInterface 是 Sparrow 提供给内部模块的调试控制接口。

它主要做两件事：
- 通过 Binder 客户端向 Debug 服务发送调试命令。
- 提供定时器测试和远程 shell 开关等调试能力。

## 2. 什么时候会用到
当需要在业务流程中触发调试动作（如测试定时器、开关远程 shell）时会用到。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `DebugInterface.h` | 调试接口 |
| 库文件 | `libdebugapi.so` | Debug 对外接口库 |

**典型流程**

1. 获取接口实例
	- 调用 `DebugInterface::GetInstance()` 获取单例对象。
	- 初始化阶段会自动连接 `SRV_NAME_DEBUG_MODULE`。

2. 调用调试命令
	- 定时器测试：`AddTimerInOneSec()`、`DelTimerInOneSec()`。
	- 自定义定时器：`AddCustomTimer()`、`DelCustomTimer()`。
	- 远程 shell：`EnableRemoteShell()`、`DisableRemoteShell()`。

3. 处理返回值
	- 所有接口返回 0 表示成功，非 0 表示失败。

**简单示例**

```cpp
#include "DebugInterface.h"

DebugInterface* dbg = DebugInterface::GetInstance();
if (dbg == nullptr) {
    return -1;
}

dbg->AddTimerInOneSec();
dbg->AddCustomTimer(3, 1000, 2000);
dbg->EnableRemoteShell();
```

**参考代码**
- 接口定义: [DebugInterface.h](../../PrivateAPIs/DebugInterface.h)
- 接口实现: [DebugInterface.cpp](../../PrivateAPIs/DebugInterface.cpp)

## 4. 要注意什么
- 该接口是命令转发层，不包含调试业务实现逻辑。
