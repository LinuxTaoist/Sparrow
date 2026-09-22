# SprLog

## 1. 这是做什么的

SprLog 是 Sparrow 提供给业务模块的日志接口层。

它提供统一的日志宏和日志对象接口。业务代码只负责记录日志，日志级别、输出位置、文件命名和文件轮转由配置及后台日志服务处理。

## 2. 什么时候会用到

当业务模块需要输出以下信息时使用 SprLog：

| 场景 | 示例 |
|------|------|
| 调试信息 | 状态、参数、流程跟踪 |
| 运行信息 | 服务启动、连接建立、任务完成 |
| 告警信息 | 重试、资源不足、配置异常 |
| 错误信息 | 文件打开失败、请求处理失败 |

## 3. 怎么用

### 3.1 快速使用

**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `SprLog.h` | 日志接口 |
| 库文件 | `libsprlog.so` | 日志库 |

只需要定义日志标签，然后使用日志宏：

```cpp
#include "SprLog.h"

#define LOG_TAG "MyModule"

SPR_LOGI("service started\n");
SPR_LOGW("warning message\n");
SPR_LOGE("error occurred: %s\n", error.c_str());
```

如果没有调用 `SPR_INIT()`，日志默认按照当前进程的可执行文件名选择配置；找不到对应配置时使用 `[output.default]`。

这是推荐的大多数场景使用方式，不需要修改代码之外的内容。

### 3.2 配置整个进程

在 `sprlog.conf` 中定义与可执行文件名对应的输出段，可以调整整个进程的日志策略，而不需要重新编译业务代码。

例如，可执行文件名为 `myservice`：

```ini
[output.myservice]
level = info
output = file
file_name = service.log
file_path = /var/log/myservice
```

常用配置是 `level`、`output`、`file_name` 和 `file_path`。配置由日志服务加载，修改后按系统启动方式重启或重新加载日志服务使其生效。

### 3.3 配置多个模块

当同一进程中的不同模块需要使用不同的日志文件或日志级别时，在程序启动早期调用 `SPR_INIT()`：

```cpp
#define LOG_TAG "Network"

SPR_INIT("NetworkService");
SPR_LOGI("network service started\n");
```

然后配置对应模块：

```ini
[output.default]
level = info
output = file
file_name = main.log
file_path = /tmp/sprlog/main

[output.NetworkService]
level = debug
output = file
file_name = network.log
file_path = /tmp/sprlog/network
```

调用 `SPR_INIT("NetworkService")` 后，日志优先使用 `[output.NetworkService]`；如果该段不存在，则回退到 `[output.default]`。

### 3.4 代码接口速查

| 接口 | 作用 | 建议 |
|------|------|------|
| `SPR_LOGD()` | Debug 日志 | 开发调试使用 |
| `SPR_LOGI()` | Info 日志 | 常规运行信息 |
| `SPR_LOGW()` | Warn 日志 | 可恢复异常或风险 |
| `SPR_LOGE()` | Error 日志 | 错误信息 |
| `SPR_INIT()` | 设置进程日志模块 | 多模块场景使用 |

这些接口适合临时调试或特殊场景。生产环境通常优先使用配置文件。

### 3.5 日志文件命名

不需要自定义文件名时，只配置 `file_name` 即可：

```ini
file_name = service.log
```

未配置 `file_name_format` 时使用默认格式 `BN.FX`，当前活跃文件为：

```text
service.log
```

文件轮转由日志服务负责，历史文件使用 `.1`、`.2` 等后缀：

```text
service.log
service.log.1
service.log.2
```

`file_name_format` 只负责生成当前活跃文件名，不负责生成轮转序号。

### 3.6 文件名 Token

只有需要区分启动时间或启动实例时，才需要配置 `file_name_format`：

```ini
file_name = service.log
file_name_format = BN_ST.FX
```

当前支持的 Token：

| Token | 含义 | 建议用途 |
|-------|------|----------|
| `BN` | 基础文件名 | 通常保留 |
| `FX` | 文件扩展名 | 通常保留 |
| `ST` | 进程启动时的本地时间 | 需要区分启动批次时使用 |
| `SI` | 系统启动标识的短值 | 特殊诊断场景使用 |
| `MH` | 单调时钟运行小时数 | 时间异常时辅助定位 |

普通场景推荐只使用 `BN`、`FX` 和 `ST`。Token 可以自由组合，例如：

```ini
file_name_format = BN_SI_MH_ST.FX  // example_58C2_0141_260922111336.log
```

### 3.7 经典场景

| 场景 | 描述 | 实现步骤 |
|------|------|----------|
| 快速接入日志 | 先把业务日志打出来，尽快跑通流程。 | 1. 定义 `LOG_TAG`。<br>2. 直接使用 `SPR_LOGD/I/W/E()`。 |
| 文件输出 | 设备或服务器上需要长期保存日志文件。 | 1. 配置 `output = file`。<br>2. 设置 `file_name` 和 `file_path`。<br>3. 按需补充轮转参数。 |
| 多模块分流 | 同一进程里的不同模块，日志级别或文件不同。 | 1. 程序早期调用 `SPR_INIT()`。<br>2. 为不同模块配置 `[output.<module>]`。 |
| 标准输出 | 调试期或容器环境，日志直接进终端。 | 1. 配置 `output = stdout`。<br>2. 按模块设置 `level`。 |

## 4. 要注意什么

### 4.1 日志路由规则

日志配置按以下规则选择：

```text
调用 SPR_INIT("ModuleName")：
    [output.ModuleName]
        ↓ 不存在时
    [output.default]

未调用 SPR_INIT()：
    [output.<可执行文件名>]
        ↓ 不存在时
    [output.default]
```

### 4.2 `SPR_INIT()` 的全局影响

`SPR_INIT()` 是进程级设置，不是线程级、对象级或单条日志级设置。

调用后，当前进程后续所有日志都会使用新的模块名查找配置，包括其他模块产生的日志。因此：

1. 必须在第一条日志输出之前调用；
2. 一个进程通常只调用一次；
3. 没有多个日志输出位置的需求时，不要调用它，使用默认路由更安全。

### 4.3 缓冲长度和日志截断

- **`LOG_TAG` 长度**：上限 12 个字符，超过会触发编译错误。
- **单条日志长度**：有效范围为 1-1024，默认值为 512。
- **超长日志**：格式化结果超过缓冲长度时会被截断，并追加 `TRUNCATED` 标记。

如果日志内容可能较长，建议拆分为多条日志，而不是无限增大单条日志缓冲。

### 4.4 文件轮转

文件输出时：

- `file_capacity_mb` 控制单个日志文件的容量；
- `file_num` 控制保留的历史文件数量；
- 当前文件使用 `file_name` 和 `file_name_format` 生成；
- 历史文件由日志服务追加 `.1`、`.2` 等轮转后缀。

### 4.5 日志服务依赖

SprLog 依赖 LogManagerSrv 提供后台日志处理。建议先启动 LogManagerSrv，再启动业务进程。

如果日志服务未启动，日志可能无法正常写入后台文件。

## 5. 参考

- 日志接口：[SprLog.h](../../../3rdAdapter/SprLog.h)
- 日志实现：[SprLog.cpp](../../../3rdAdapter/SprLog.cpp)
- 使用示例：[15_SprLog.cc](../../../Examples/15_SprLog.cc)
