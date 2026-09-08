# Property

## 1. 这是做什么的
Property 是 Sparrow 对外提供的属性读写接口。

它主要做两件事：
- 通过 Binder 客户端向 `property_service` 发送属性读写请求。
- 提供单条属性读写、批量属性获取和属性列表调试输出能力。

## 2. 什么时候会用到
当业务模块需要跨进程读取或更新系统属性（如配置项、运行时参数）时会用到。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | `Property.h` | 属性接口 |
| 库文件 | `libpropertyapi.so` | Property 对外接口库 |

**典型流程**

1. 获取接口实例
	- 调用 `Property::GetInstance()` 获取单例对象。
	- 初始化阶段会自动连接 `property_service`。

2. 执行属性读写
	- 写属性调用 `SetProperty(key, value)`。
	- 读属性调用 `GetProperty(key, value, defaultValue)`。

3. 批量查询或调试输出
	- 调用 `GetProperties(properties)` 获取全部属性。
	- 调用 `DumpProperties()` 触发服务侧输出属性信息。

**简单示例**

1. 设置和读取属性

```cpp
#include "Property.h"

Property* prop = Property::GetInstance();
if (prop == nullptr) {
    return -1;
}

if (prop->SetProperty("persist.demo.mode", "on") != 0) {
    return -1;
}

std::string value;
if (prop->GetProperty("persist.demo.mode", value, "off") != 0) {
    return -1;
}
```

2. 批量获取属性

```cpp
std::map<std::string, std::string> allProps;
if (prop->GetProperties(allProps) == 0) {
    // 使用 allProps
}
```

**参考代码**
- 接口定义: [Property.h](../../../PublicAPIs/Property.h)
- 接口实现: [Property.cpp](../../../PublicAPIs/Property.cpp)

## 4. 要注意什么
- 构造阶段若 `InitializeClientBinder("property_service", ...)` 失败，接口会被置为不可用，后续方法直接返回 -1，参考 [Property.cpp:47](../../../PublicAPIs/Property.cpp#L47)
- `GetProperty`失败时， 会返回 `defaultValue`，参考 [Property.cpp:98](../../../PublicAPIs/Property.cpp#L98)

