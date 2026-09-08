# ConfigManagerSrv

## 1. 这是做什么的
ConfigManagerSrv 是 Sparrow 的配置存储服务，负责通过 Binder 统一处理配置读写、分层回退、版本号管理和备份。

当前实现支持：
- 字符串配置与 typed 配置（bool/int32/int64/float/double）。
- 三层作用域管理：default、factory、user。
- 按优先级读取生效值：user > factory > default。
- revision 递增管理与元信息查询（scope/revision）。
- sqlite 持久化与数据库备份。

## 2. 什么时候会用到
当业务模块需要按 namespace/key 保存配置，并且需要作用域分层、版本追踪或备份能力时，就会用到它。

## 3. 怎么用
在使用配置功能时，外部业务模块只需要调用 PublicAPIs 的 Config 接口，对 ConfigManagerSrv 内部实现无感知。

接口入口，请看：
- [Config.h](../../../../PublicAPIs/Config.h)
- [Config.cpp](../../../../PublicAPIs/Config.cpp)

相关实现入口：
- [ConfigManagerSrv](../../../../Components/Basics/ConfigManagerSrv)
- [config_manager.conf](../../../../Configs/General/config_manager.conf)
- [ConfigManagerWatch](../../../../Debug/InfraWatch/ConfigManagerWatch.cpp)

## 4. 要注意什么
- ConfigManagerSrv 是服务端存储与分发模块，不是业务模块直接拼 Parcel 的调用入口。
- `SetValueWithScope` 可以写入指定 scope，普通读取会按 `user > factory > default` 返回生效值。
- `GetMeta` 只返回 scope 和 revision，不返回 value 本身。
- typed 读取会校验类型和字节长度，不匹配会返回失败。
- `ListNamespace` 当前只返回字符串类型配置项。
- 默认配置文件名是 `config_manager.conf`，默认数据库路径是 `/tmp/config_manager.db`，默认备份路径是 `/tmp/config_manager.db.bak`。
- 如果 `config_manager.conf` 不存在，服务会使用内置默认路径继续运行。
