# SprEnumHelper

## 1. 这是做什么的
SprEnumHelper 是 Sparrow 的枚举与错误码文本转换工具。

它主要做两件事：
- 根据错误码计算错误等级，并把等级或错误码转换为可读字符串。
- 把模块 ID、Binder 命令、日志级别、启动类型、电源相关类型等枚举值转换为文本，便于日志打印和调试定位。

## 2. 什么时候会用到
当模块需要把内部整型枚举或错误码输出为可读文本时会用到，常用于日志、调试输出、测试断言和故障诊断。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `CommonErrorCodes.h` <br> `CoreTypeDefs.h` <br> `SprEnumHelper.h` | 错误码定义 <br> Core 枚举定义 <br> 转换接口声明 |
| 库文件 | `libsprcore.so` | 包含 SprEnumHelper 实现 |

**典型流程**

1. 引入头文件并使用 `InternalDefs` 命名空间
	- 在需要打印可读文本的模块中包含 `SprEnumHelper.h`。
	- 直接调用对应 `GetSprXXXText()` 接口完成转换。

2. 错误码和错误等级转换
	- 使用 `GetSprErrorLevel(errorCode)` 计算错误等级。
	- 使用 `GetSprErrorLevelText(level)` 和 `GetSprErrorText(errorCode)` 输出可读文本。

3. 业务枚举转换
	- 按枚举类型调用对应接口，如 `GetSprModuleIDText()`、`GetSprLogLevelText()`、`GetSprProxyBinderCmdText()`。
	- 对越界值，接口会返回统一兜底文本（如 `UNDEFINED`），便于快速识别非法输入。

**简单示例**

1. 错误码转换

```cpp
#include "CommonErrorCodes.h"
#include "SprEnumHelper.h"

using namespace InternalDefs;

int32_t errorCode = ERR_SHM_READ_FAILED;
int32_t level = GetSprErrorLevel(errorCode);
std::string levelText = GetSprErrorLevelText(level);
std::string errorText = GetSprErrorText(errorCode);

SPR_LOGE("error=%d level=%s text=%s\n",
    errorCode, levelText.c_str(), errorText.c_str());
```

2. 枚举转换

```cpp
using namespace InternalDefs;

SPR_LOGI("module=%s logLevel=%s\n",
    GetSprModuleIDText(MODULE_GENERAL).c_str(),
    GetSprLogLevelText(LOG_LEVEL_INFO).c_str());
```

**参考代码**
- 接口定义: [SprEnumHelper.h](../../../Core/SprEnumHelper.h)
- 接口实现: [SprEnumHelper.cpp](../../../Core/SprEnumHelper.cpp)
- 单元测试: [TestSprEnumHelper.cc](../../../TestCase/Internal/Core/TestSprEnumHelper.cc)

## 4. 要注意什么
- 错误等级由 `(0 - errorCode) % 50` 计算，输入错误码需遵循项目错误码编码规则，参考 [SprEnumHelper.cpp:36](../../../Core/SprEnumHelper.cpp#L36)
- `GetSprErrorLevel()` 对未命中区间返回 `ERR_EVENT_LEVEL_UNKNOWN`，参考 [SprEnumHelper.cpp:51](../../../Core/SprEnumHelper.cpp#L51)
- 多数 `GetSprXXXText()` 对越界值统一返回 `UNDEFINED`，参考 [SprEnumHelper.cpp:60](../../../Core/SprEnumHelper.cpp#L60) [SprEnumHelper.cpp:185](../../../Core/SprEnumHelper.cpp#L185)
- `GetSprModuleIDText()` 对越界模块 ID 不返回 `UNDEFINED`，而是返回 `MODULE_0X%X` 格式文本，参考 [SprEnumHelper.cpp:123](../../../Core/SprEnumHelper.cpp#L123)
