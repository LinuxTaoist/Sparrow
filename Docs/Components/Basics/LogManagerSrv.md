# LogManagerSrv

## 1. 这是做什么的
LogManagerSrv 是 Sparrow 的日志落盘服务，负责从共享缓存中读取日志数据，按配置完成级别筛选、长度校验、输出写入和日志轮转。


## 2. 什么时候会用到
当系统需要统一接收业务进程日志并保存为标准输出或日志文件时，就会用到它。

## 3. 怎么用
在使用日志功能时，外部业务模块只需要调用SprLog接口，对 LogManagerSrv 无感知。

接口使用文档，请看：
- [SprLog.md](../../../Docs/3rdAdapter/SprLog.md)

相关实现入口：
- [LogManagerSrv](../../../Components/Basics/LogManagerSrv)
- [SprLog](../../../3rdAdapter/SprLog.h)

## 4. 要注意什么
- LogManagerSrv 只负责日志消费和保存，不是业务模块直接调用的打印接口。
- 日志写入链路依赖共享内存和信号量，业务进程与日志服务要使用同一套默认配置和服务名约定。
- 文件模式下会自动轮转日志文件，旧文件名会按后缀递增保存。
- 如果日志单帧长度超过配置限制，该帧会被拒绝写入。
- 默认配置文件名是 `sprlog.conf`，默认日志目录是 `/tmp/sprlog`。
