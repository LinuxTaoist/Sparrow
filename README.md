# Sparrow：Linux C++ 嵌入式物联网中间件

👉 [[English]](README.en.md)

## 📌 项目定位
`Sparrow`是一款基于**Epoll事件驱动模型**的`Linux C++`嵌入式物联网中间件框架。采用**微服务+插件化架构**，支持多硬件平台无缝移植，致力于为智能硬件、车载系统、物联网网关等场景提供高效、稳定、易扩展的应用开发底座。

## 🌟 核心亮点
- **开箱即用**：核心框架无强依赖第三方库，多平台一键编译，快速部署落地
- **多平台适配**：代码与配置完全分离，实现嵌入式Linux平台快速移植
- **服务化治理**：内置系统服务集群，支持进程守护、状态监控、自动拉起，保障服务高可用
- **远程运维**：集成`rshellx`远程工具，实现Shell执行、文件传输，设备端问题快速定位
- **实时调试**：调试节点、属性动态读写、日志实时查看，开发调试全流程提效
- **异常捕获**：程序崩溃自动记录堆栈日志，结合分级日志系统，问题溯源零障碍
- **质量保障**：集成`cppcheck`静态扫描、`googletest`单元测试
- **生态扩展**：内置SQLite/HTTP/MQTT协议栈，支持第三方库适配与插件动态加载卸载

## 🚀 3分钟快速上手
### 1. 环境要求
- C++11 及以上版本
- Linux 系统

### 2. 编译项目
```bash
# 进入构建目录执行脚本
$ cd Sparrow/Build
$ ./rebuild_default.sh

# 编译产物在 Release 目录
$ cd Release
$ ls
Bin  Cache  Include  Lib
```

### 3. 启动服务
```bash
# 进入二进制目录，启动服务管理器（自动拉起所有核心服务）
$ cd Release/Bin
$ ./servicemanagersrv
```

### 4. 实时查看日志
```
$ tail -f /tmp/sprlog/sparrow.log
07-20 10:12:59.104  70543      BinderM D:   80 Add service info(8336, powermanagersrv)
07-20 10:12:59.107  70550     EpollSch D:   36 ===========  Sparrow Epoll Start  ===========
07-20 10:12:59.112  70550     EpollSch D:   97 Poll add module 5 DebugM
07-20 10:12:59.112  70550     IpcProxy D:   74 Register observer: [0x8] [/DebugM_BymUfOrK]
07-20 10:12:59.112  70550   SprObsBase D:  241 [DebugM] - Dump common version: COMMON_TYPE_DEFS_VERSION_N1001 / COMMON_MACROS_VERSION_M1001 / CORE_TYPE_DEFS_VERSION_R1001
07-20 10:12:59.112  70550   SprObsBase D:   62 [DebugM] Start Module: DebugM, mq: /DebugM_BymUfOrK
07-20 10:12:59.112  70545  SprMediator D:  417 Register successfully! ID: 8, NAME: /DebugM_BymUfOrK, monitored = 1
07-20 10:12:59.112  70543      BinderM D:   80 Add service info(92137, debugsrv)
07-20 10:12:59.112  70545  SprMediator D:  247 Binder loop start!
07-20 10:12:59.113  70550   SprObsBase D:  255 [DebugM] Register Successfully!
```

## 📢 功能介绍
### 核心功能
| 功能          | 关键特性                                 | 说明                       |
|---------------|-----------------------------------------|----------------------------|
| 进程管理      | 进程拉起、守护、状态监控、异常重启        | 保障核心服务7×24小时稳定运行 |
| 日志管理      | 分级日志、动态调节、格式化输出、持久化    | 标准化日志体系，便捷问题排查 |
| Binder RPC    | 同步/异步通信、服务注册发现               | 跨进程高性能通信核心 |
| 消息中转      | 消息广播、订阅发布、消息队列               | 模块解耦，异步通信 |
| Property管理  | 属性读写、变更通知、配置导出、持久化       | 统一配置管理中心 |
| 电源管理      | 电源状态管控、事件通知、低功耗适配         | 嵌入式设备电源生命周期管理 |
| 插件管理      | 动态加载/卸载、模块化扩展                 | 功能灵活插拔，降低耦合 |
| 定时器管理     | 定时任务、周期任务、回调触发              | 高精度定时调度 |
| 状态监控      | 接口超时、设备状态、异常事件采集存储       | 全维度运行状态监控 |
| 线程池        | 线程复用、任务异步提交、回调通知          | 高并发任务处理 |
| 事件驱动      | Epoll事件循环、IO/定时器/信号事件统一管理  | 高并发低功耗核心 |
| 调试节点      | 调试入口注册/触发/响应                    | 在线模拟调试，无需重启设备 |
| 崩溃捕获      | 异常崩溃堆栈自动记录                      | 快速定位崩溃根因 |
| 自动化测试    | 集成gtest，核心模块单元/集成测试          | 保障代码质量，降低变更风险 |
| 静态扫描      | cppcheck代码风险检测、valgrind内存检测     | 提前规避代码缺陷 |
| 多平台适配    | 平台配置隔离、交叉编译一键切换            | 快速移植到不同硬件平台 |

### 辅助工具
| 工具          | 关键特性                                 | 说明                       |
|---------------|-------------------------------------------|----------------------------|
| rshellx       | 远程Shell执行、文件双向传输               | 无显示器设备远程运维 |
| infrawatch    | 服务状态监控、实时日志、远程调试交互       | 设备在线一站式诊断 |
| logshow       | 日志实时查看、过滤检索                    | 轻量化日志查看工具 |
| getprop/setprop | 系统属性快速读写                          | 配置动态调试 |

## 🎯 适用场景
- 智能硬件：智能摄像头、机器人控制系统、工业传感器、智能终端等嵌入式设备
- 车载系统：车载T-Box、智能座舱、车联网网关等车载终端
- 物联网网关：智能家居中控、环境监测网关、工业IoT网关等数据中转设备
- 消费电子：智能穿戴设备、健康监测仪、智能家电等轻量化智能产品

## 📂 源码结构速览
```
Sparrow/
├── 3rdAdapter                // 第三方库适配层（SQLite/日志封装）
├── 3rdParty                  // 第三方依赖库（gtest/libgo/sqlite）
├── Build                     // 编译脚本入口
├── CMakeLists.txt            // 工程顶层编译配置
├── Components                // 服务组件集
│   ├── Basics                // 核心系统服务（Binder/日志/电源/属性等）
│   └── Business              // 业务组件（OneNet MQTT云对接）
├── Configs                   // 全局配置文件
├── Core                      // 框架核心（Binder/事件调度/线程池/消息机制）
├── Debug                     // 调试工具集（infrawatch/rshellx/logshow）
├── Docs                      // 工程文档（规范/指南/架构/疑难问题）
├── Examples                  // 功能使用示例代码
├── Hardware                  // 硬件适配接口
├── LICENSE                   // 开源许可证
├── Platform                  // 多平台适配配置
├── PrivateAPIs               // 框架内部接口（Binder/调试/中介者）
├── PublicAPIs                // 对外业务接口（属性/电源/OneNet）
├── README.en.md
├── README.md
├── Release                   // 编译输出（二进制/库/头文件/缓存）
├── TestCase                  // 测试用例（内部核心/外部扩展）
├── Tools                     // 质量工具（cppcheck/valgrind）
├── Util                      // 通用工具库（序列化/JSON/回溯/算法）
├── UtilModules               // 基础功能模块（编解码/Epoll/HTTP/MQTT）
├── modules_config.cmake      // 模块编译配置
└── version.cmake             // 版本管理配置
```

## 📚 文档与资源
- [使用指南](Docs/UserManual)：包含调试节点、定时器、日志等功能的使用说明。
- [编程规范](Docs/C++编程规范.md)：统一代码风格，优先以当前项目代码为准。
- [版本管理](Docs/版本管理.md)：版本发布记录，持续更新中。
- [疑难杂症](Docs/疑难杂症.md)：常见问题解决方案，持续更新中。

## 🤝 互动交流
- Issues：提交bug、建议或参与讨论。
- 公众号：开源519
- 微信：StopCoding （备注Sparrow）

## 📄 许可
本项目使用 MIT 许可证，详见 [LICENSE](LICENSE)

---

## 💖 鼓励支持
如果您觉得本项目有意义，可以通过以下方式给予支持：
- 三连支持：Star、Fork、Watch
- 积极参与讨论：在Issues中提出问题或建议
- 公众号：开源519，关注、转发并点赞[公众号文章](https://mp.weixin.qq.com/s/DHiZ4iQJAQVrN8z4kDTiLg)
- 微信：StopCoding，加入技术交流群

既入此行，砥砺精进；积硅步以至千里，聚小流而成江海。脚步不停，终达卓越！
