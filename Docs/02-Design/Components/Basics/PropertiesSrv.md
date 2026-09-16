# PropertiesSrv

## 1. 这是做什么的
PropertiesSrv 是 Sparrow 的属性管理服务，负责属性加载、读写、广播与持久化。

当前实现主要包括：
- 属性键值对读写（SetProperty/GetProperty/GetProperties）。
- 启动时加载 default/system/vendor 属性文件。
- `persist.` 前缀属性持久化保存与回读。
- 属性变更通知及日志属性联动（log level/log length）。

## 2. 什么时候会用到
当业务模块需要跨进程共享配置参数，或需要动态调整日志等级、日志长度等运行属性时，就会用到它。

## 3. 怎么用
业务模块通过 PublicAPIs 的 Property 接口访问属性，不需要直接操作 PropertiesSrv 内部共享内存结构。

接口使用文档请看：
- [Property](../../../05-Reference/PublicAPIs/Property.md)

相关实现入口：
- [Property.h](../../../../PublicAPIs/Property.h)
- [Property.cpp](../../../../PublicAPIs/Property.cpp)
- [main_properties.cpp](../../../../Components/Basics/PropertiesSrv/main_properties.cpp)
- [PropertyManager.h](../../../../Components/Basics/PropertiesSrv/PropertyManager.h)
- [PropertyManager.cpp](../../../../Components/Basics/PropertiesSrv/PropertyManager.cpp)
- [PropertyManagerHub.h](../../../../Components/Basics/PropertiesSrv/PropertyManagerHub.h)
- [default.prop](../../../../Configs/General/default.prop)
- [system.prop](../../../../Configs/General/system.prop)

## 4. 要注意什么
- PropertiesSrv 基于 SharedBinaryTree 共享内存结构，默认最大容量是 128KB。
- 属性加载顺序是 default -> system -> vendor -> persist，后加载可覆盖先加载。
- `ro.` 前缀属性在已存在时不允许被修改。
- `persist.` 前缀属性会额外写入持久区，进程重启后仍可恢复。
- `system.log.level` 和 `system.log.length` 变更会触发日志系统参数联动。
- 当属性不存在时，GetProperty 会返回失败并输出调用方提供的默认值。