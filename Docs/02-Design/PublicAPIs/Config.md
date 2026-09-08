# Config

## 1. 这是做什么的
Config 是 Sparrow 对外提供的配置读写接口。

它主要做两件事：
- 通过 Binder 客户端向 ConfigManager 服务发送配置读写请求。
- 提供字符串与 typed 配置读写、元信息查询、命名空间列表和备份能力。

## 2. 什么时候会用到
当业务模块需要跨进程保存或读取配置，并且希望使用 namespace、scope、revision 进行配置管理时会用到。

## 3. 怎么用
**依赖**

| 类型 | 依赖项 | 说明 |
|------|--------|------|
| 头文件 | Config.h | 配置接口 |
| 库文件 | libconfigapi.so | Config 对外接口库 |

**典型流程**

1. 获取接口实例
	- 调用 Config::GetInstance() 获取单例对象。
	- 初始化阶段会自动连接 SRV_NAME_CONFIG_MANAGER 服务。

2. 执行配置读写
	- 字符串配置可调用 SetValue 或 SetValueWithScope。
	- typed 配置可调用 SetIntValue、SetBoolValue、SetFloatValue 等接口。
	- 读取时可用 GetValue 或 typed Get 接口，并可携带 defaultValue。

3. 查询元信息或维护数据
	- 调用 GetMeta 获取当前生效 scope 和 revision。
	- 调用 ListNamespace 获取命名空间下字符串配置项。
	- 调用 Backup 触发服务端数据库备份。

**简单示例**

1. 字符串配置读写

```cpp
#include "Config.h"

Config* cfg = Config::GetInstance();
if (cfg == nullptr) {
    return -1;
}

int32_t revision = 0;
if (cfg->SetValueWithScope("net", "apn", "cmnet", CONFIG_SCOPE_USER, revision) != 0) {
    return -1;
}

std::string value;
int32_t scope = CONFIG_SCOPE_DEFAULT;
if (cfg->GetValue("net", "apn", value, "default_apn", scope) != 0) {
    return -1;
}
```

2. typed 配置读写

```cpp
int32_t retry = 0;
if (cfg->SetIntValue("retry_interval_sec", 30, CONFIG_SCOPE_USER, "cloud") != 0) {
    return -1;
}

if (cfg->GetIntValue("retry_interval_sec", retry, 10, "cloud") != 0) {
    return -1;
}
```

**参考代码**
- 接口定义: [Config.h](../../../PublicAPIs/Config.h)
- 接口实现: [Config.cpp](../../../PublicAPIs/Config.cpp)

## 4. 要注意什么
- 构造阶段若 InitializeClientBinder(SRV_NAME_CONFIG_MANAGER, ...) 失败，接口会被置为不可用，后续方法直接返回 -1，参考 [PublicAPIs/Config.cpp](../../../PublicAPIs/Config.cpp#L47)
- key 版 SetValue 会默认写入 DEFAULT_NAMESPACE，且 scope 默认走 USER 层，参考 [PublicAPIs/Config.cpp](../../../PublicAPIs/Config.cpp#L86)
- 所有同步请求通过 ProcLockGuard(gPMutex, gTMutex) 串行保护，避免并发请求冲突，参考 [PublicAPIs/Config.cpp](../../../PublicAPIs/Config.cpp#L145)
- GetValue 和 typed Get 在失败时会回填 defaultValue，但接口返回值仍是 -1，需要同时判断返回值和输出参数，参考 [PublicAPIs/Config.cpp](../../../PublicAPIs/Config.cpp#L193)
- GetMeta 只返回 scope 和 revision，不返回 value，参考 [PublicAPIs/Config.cpp](../../../PublicAPIs/Config.cpp#L384)
- 当前 PublicAPI 没有对外 Delete 接口，配置删除不在业务侧开放能力范围内。
