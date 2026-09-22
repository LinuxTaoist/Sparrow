# Examples

本目录用于汇总 Sparrow 的示例程序，按功能直接定位对应用法，便于快速查阅和验证。

## 目录说明

| 示例 | 功能 |
| --- | --- |
| [01_ProcGuard.cc](01_ProcGuard.cc) | 进程守护与保护示例，演示进程存活与恢复。 |
| [02_SprDirWatch.cc](02_SprDirWatch.cc) | 目录监听示例，演示文件/目录变更监控。 |
| [03_UserMain.cc](03_UserMain.cc) | 用户主程序入口示例，适合自定义启动流程。 |
| [04_DebugSprCore.cc](04_DebugSprCore.cc) | 核心框架调试示例，适合查看核心能力用法。 |
| [05_DebugMsg.cc](05_DebugMsg.cc) | 消息机制调试示例，演示消息收发与处理。 |
| [06_DebugBinder.cc](06_DebugBinder.cc) | Binder 通信示例，演示模块间直接通信。 |
| [07_TcpServer.cc](07_TcpServer.cc) | TCP 服务端示例，演示事件驱动的网络服务。 |
| [08_TcpClient.cc](08_TcpClient.cc) | TCP 客户端示例，演示客户端连接与收发。 |
| [09_MQueue.cc](09_MQueue.cc) | 消息队列示例，演示 MQueue 交互。 |
| [10_HttpBytes.cc](10_HttpBytes.cc) | HTTP 字节流处理示例，适合协议解析入门。 |
| [11_HttpServer.cc](11_HttpServer.cc) | HTTP 服务端示例，演示基础 HTTP 服务。 |
| [12_Sqlite.cc](12_Sqlite.cc) | SQLite 使用示例，演示数据库读写操作。 |
| [13_DebugCodecX.cc](13_DebugCodecX.cc) | CodecX 编解码示例，适合查看结构化数据处理。 |
| [14_DebugHwHal.cc](14_DebugHwHal.cc) | 硬件抽象层示例，便于了解平台硬件接口。 |
| [15_SprLog.cc](15_SprLog.cc) | 日志输出示例，演示 SprLog 的使用方式。 |

## 使用方式

1. 打开 [Default_modules_config.cmake](../Platform/Default/Build/Options/Default_modules_config.cmake)，取消对应行注释：

```cmake
set(BUILD_EXAMPLES ON)
```

2. 在工程根目录执行：

```bash
cd Build
./build_default.sh
```

3. 编译完成后，直接在输出目录中运行对应示例二进制。
