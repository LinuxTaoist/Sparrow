# SprLog

## 1. 这是做什么的
SprLog 是 Sparrow 提供给业务模块的日志接口层。

它对外提供统一日志宏和日志对象接口，负责把业务日志格式化后写入日志链路。业务方只关心“写什么日志”，不需要关心底层共享缓存和日志服务处理细节。

## 2. 什么时候会用到
当业务模块需要打印调试信息、运行信息、告警信息或错误信息时会用到。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `SprLog.h` | 日志接口 |
| 库文件 | `libsprlog.so` | 日志库 |

**典型流程**

1. 在模块中定义日志标签 (必须)
	- 定义 `LOG_TAG`，作为该模块日志的统一标识。

2. 使用日志宏打印日志 (必须)
	- 使用 `SPR_LOGD`、`SPR_LOGI`、`SPR_LOGW`、`SPR_LOGE` 输出不同级别日志。

3. 按需调整日志行为 (可选)
	- 通过 `SetLevel()` 控制输出级别。
	- 通过 `SetLength()` 控制单条日志格式化缓冲长度。

参考代码：
- 日志接口定义: [SprLog.h](../../3rdAdapter/SprLog.h)

## 4. 要注意什么
- LOG_TAG 长度上限是 12 个字符。超过后会在编译期触发 static_assert 失败。[SprLog.h:26](../../3rdAdapter/SprLog.h#L26)
- SetLength() 的有效上限是 1024，默认值是 512。length <= 0 时会回退到默认值 512。[SprLog.cpp:40](../../3rdAdapter/SprLog.cpp#L40)
- 格式化结果超过当前缓冲大小或超过 1024 时，会被截断并追加 TRUNCATED 提示。[SprLog.h:27](../../3rdAdapter/SprLog.h#L27)     \
级别枚举顺序是：
    - LOG_LEVEL_MIN(0): 不输出任何日志
    - LOG_LEVEL_ERROR(1): 只输出ERROR级别日志
    - LOG_LEVEL_WARN(2): 输出ERROR和WARN级别日志
    - LOG_LEVEL_INFO(3): 输出ERROR、WARN和INFO级别日志
    - LOG_LEVEL_DEBUG(4): 输出ERROR、WARN、INFO和DEBUG级别日志
    - LOG_LEVEL_BUTT(5): 输出所有级别日志
