# SprDirWatch

## 1. 这是做什么的
SprDirWatch 是 Sparrow 对 Linux inotify 的轻量封装，用于监听目录文件变化。

它主要做两件事：
- 创建并持有 inotify 监听文件描述符，供外部事件循环统一管理。
- 提供目录监听注册能力，并在对象析构时自动清理已注册的 watch。

## 2. 什么时候会用到
当模块需要感知目录内文件创建、删除、修改等事件，并希望把监听句柄接入现有 epoll/select 循环时会用到。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `inotify.h`<br> `SprDirWatch.h` | 目录监听封装接口 <br> inotify 事件掩码定义 |
| 库文件 | `libsprcore.so` | 包含 SprDirWatch 实现 |

**典型流程**

1. 创建监听对象
	- 构造 `SprDirWatch`，内部调用 `inotify_init()` 创建监听 fd。
	- 通过 `GetInotifyFd()` 获取 fd，接入模块自己的事件循环。

2. 注册目录监听
	- 调用 `AddDirWatch(path, mask)` 注册监听目录。
	- `mask` 使用 inotify 标准掩码（如 `IN_CREATE`、`IN_DELETE`、`IN_MODIFY`）。

3. 在事件循环中读取并解析事件
	- 当 `GetInotifyFd()` 可读时，读取 `inotify_event` 数据并按业务逻辑处理。
	- 模块退出时销毁 `SprDirWatch` 对象，已注册 watch 会自动清理。

**简单示例**

1. 创建对象并注册监听

```cpp
#include "SprDirWatch.h"
#include <sys/inotify.h>

SprDirWatch watcher;
int32_t inotifyFd = watcher.GetInotifyFd();
if (inotifyFd < 0) {
    return -1;
}

int32_t wd = watcher.AddDirWatch("/tmp/config", IN_CREATE | IN_DELETE | IN_MODIFY);
if (wd < 0) {
    return -1;
}
```

2. 在事件循环中读取事件

```cpp
char buffer[4096] = {0};
ssize_t len = read(inotifyFd, buffer, sizeof(buffer));
if (len > 0) {
    // 按 inotify_event 结构遍历 buffer，处理目录变化事件
}
```

**参考代码**
- 接口定义: [SprDirWatch.h](../../../Core/SprDirWatch.h)
- 接口实现: [SprDirWatch.cpp](../../../Core/SprDirWatch.cpp)

## 4. 要注意什么
- `SprDirWatch` 构造时会调用 `inotify_init()`，失败时 `GetInotifyFd()` 可能小于 0，使用前需判定，参考 [SprDirWatch.cpp:30](../../../Core/SprDirWatch.cpp#L30)
- `AddDirWatch()` 失败返回 -1，调用方需要处理失败路径，参考 [SprDirWatch.cpp:46](../../../Core/SprDirWatch.cpp#L46)
- 析构时会遍历已注册 watch 并调用 `inotify_rm_watch()` 清理，然后关闭 inotify fd，参考 [SprDirWatch.cpp:38](../../../Core/SprDirWatch.cpp#L38) [SprDirWatch.cpp:41](../../../Core/SprDirWatch.cpp#L41)
- `DelDirWatch()` 是私有接口，外部只通过对象生命周期统一释放 watch，参考 [SprDirWatch.h:37](../../../Core/SprDirWatch.h#L37)
