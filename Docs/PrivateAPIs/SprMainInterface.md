# SprMainInterface

## 1. 这是做什么的
SprMainInterface 是 Sparrow 提供的主入口封装接口。

它主要做两件事：
- 通过 `SPR_ENTRY()` 宏把用户入口函数注册到框架主流程。
- 在统一 `main()` 中完成信号处理、进程初始化和事件循环启动。

## 2. 什么时候会用到
当业务需要接入 Sparrow 运行时并使用统一启动流程时会用到。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `SprMainInterface.h` | 入口宏定义 |
| 库文件 | `libsprmain.so` | 主入口封装库 |

**典型流程**

1. 在业务代码中声明入口
	- 使用 `SPR_ENTRY()` 定义 `SprUserEntry()`。

2. 在入口函数中注册业务逻辑
	- 在 `SprUserEntry()` 中完成模块初始化与启动。

3. 由框架主函数接管运行
	- 框架 `main()` 自动执行初始化，然后调用 `pSprUserEntry()` 并进入 epoll 循环。

**简单示例**

```cpp
#include "SprMainInterface.h"

SPR_ENTRY()
{
    // 用户业务初始化逻辑
    // 例如: 初始化模块、注册消息处理等
}
```

**参考代码**
- 接口定义: [SprMainInterface.h](../../PrivateAPIs/SprMainInterface.h)
- 接口实现: [SprMainInterface.cpp](../../PrivateAPIs/SprMainInterface.cpp)

## 4. 要注意什么
- 必须实现 `SPR_ENTRY()`，否则 `SPR_ENTRY_IMPLEMENT_CHECK()` 会触发 `NOT_IMPLEMENT_SPR_ENTRY()` 检查失败，参考 [SprMainInterface.cpp:34](../../PrivateAPIs/SprMainInterface.cpp#L34)
- 框架主流程会先调用 `SprProcPrepare::Init(proc)` 再执行用户入口，最后进入 `EpollLoop()`，参考 [SprMainInterface.cpp:64](../../PrivateAPIs/SprMainInterface.cpp#L64)
- `SPR_ENTRY()` 宏会在静态初始化阶段把 `SprUserEntry` 绑定到全局函数指针，参考 [SprMainInterface.h:29](../../PrivateAPIs/SprMainInterface.h#L29)
- 该接口负责流程编排，业务模块不要重复实现 `main()`。
