# Sparrow：Linux C++ 嵌入式中间件

## 📌 项目定位
`Sparrow`是一款基于事件驱动模型的`Linux`中间件框架。采用微服务架构，旨在快速构建高效的`Linux C++`中间件应用框架。


## 🌟 核心亮点
- **开箱即用**： 基础功能不依赖第三方库，开箱即用，快速部署。
- **远程定位**： 提供远程工具`rshellx`，实现远程`shell`执行，快速定位问题。
- **配置分离**： 配置文件与代码分离，轻松实现项目差异化。
- **状态监控**： 提供状态监控功能，可记录如接口执行超时、设备状态变化等多种优先级事件。
- **静态扫描**： 提供工具静态扫描`shell`脚本，配合`cppcheck`，实现代码风险扫描。
- **实时调试**： 集成调试节点功能，方便开发阶段触发模拟、调试。
- **崩溃捕获**： 程序异常崩溃时，自动记录崩溃堆栈，便于问题排查。
- **单元测试**： 提供测试用例，配合`googletest`，方便代码改动后的全功能验证。
- **插件化编程**： 支持插件化编程，实现功能模块的动态加载与卸载。

等，更多功能详见功能介绍。

## 🚀 3分钟快速上手
### 1. 环境要求
- C++11 及以上版本
- Linux 系统

### 2. 编译项目
```bash
# 进入构建目录执行脚本
$ cd Sparrow/Build
$ ./final_general_build.sh

# 编译产物在 Release 目录
$ cd Release
$ ls
Bin  Cache  Include  Lib
```

<<<<<<< HEAD
## 源码目录

=======
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

###  核心功能
| 功能          | 关键特性                                 | 说明                       |
|---------------|-----------------------------------------|----------------------------|
| 进程管理      | 进程拉起、进程守护、进程状态监控           | 确保关键服务稳定运行         |
| 日志管理      | 日志等级动态调节、日志格式化输出           | 方便问题定位与调试           |
| RPC通信      | 支持同步、异步通信                        |                             |
| 消息中转      | 支持消息广播、消息订阅                    | 实现模块间解耦               |
| Property管理  | 支持属性读写、属性变更通知、属性导出       | 实现配置管理                 |
| 电源管理      | 支持电源状态管理、电源事件通知             | 实现设备电源管理             |
| 插件管理      | 支持插件动态加载、卸载                    | 实现功能模块的动态扩展       |
| 定时器管理     | 支持定时任务、定时任务回调                | 实现定时任务管理             |
| 状态监控      | 支持状态异常捕获、存储                    |                            |
| 线程池        | 支持线程池创建、任务提交、任务回调          |                           |
| 调试节点      | 支持各模块调试入口注册、注销、触发响应       | 方便调试                   |
| 测试用例      | 配合gtest，方便代码改动后的全功能验证      | 提高代码质量，降低风险       |
| 崩溃捕获      | 程序异常崩溃时，自动记录崩溃堆栈，便于问题排查 | 提高代码质量，降低风险     |
| 静态扫描      | 静态代码扫描、代码风险分析                 | 提高代码质量，降低风险     |

###  辅助工具
| 工具          | 关键特性                                 | 说明                       |
|---------------|-------------------------------------------|----------------------------|
| rshellx       | 远程shell执行、远程文件传输               | 硬件场景下快速定位问题     |
| infrawatch    | 状态监控、实时日志、远程调试              | 在线定位问题，无需重启设备 |


## 🎯 适用场景
- 智能硬件：如智能灯泡、智能插座、智能摄像头、机器人控制系统等，适用于需要高效通信与控制的设备。
- 消费电子：例如智能手表、健康监测手环、智能电视等，支持数据收集、分析及用户界面管理。
- 车载系统：包括车载T-Box、智能座舱等，提供导航、娱乐、电话连接等功能，增强驾驶体验。
- 物联网（IoT）：涵盖智能家居、环境检测、物联网网关等，实现设备互联、数据收集与处理。


## 📂 源码结构速览
>>>>>>> feature-versionmanager
```
Sparrow/
├── 3rdAdapter                // 第三方软件适配接口
├── 3rdParty                  // 第三方软件库
├── Build                     // 构建脚本
├── CMakeLists.txt
├── Components                // 独立组件
├── Core                      // 核心框架
├── Debug                     // 调试工具
├── Docs                      // 文档
├── Examples                  // 示例
├── Hardware                  // 硬件适配接口
├── LICENSE                   // 许可证
├── PrivateAPIs               // 内部公开接口
├── ProjectConfigs            // 项目配置
├── PublicAPIs                // 外部公开接口
├── README.en.md
├── README.md
├── Release                   // 发布路径
├── StaticScans               // 静态扫描修复纪录
├── TestCase                  // 测试用例
├── Tools                     // 常用辅助脚本/可执行程序
├── Util                      // 通用工具函数
├── UtilModules               // 通用模块库
├── modules_configs.cmake     // 模块配置
└── version.cmake             // 版本信息
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


<<<<<<< HEAD
### 启动服务
```shell
$ ./servicemanagersrv
192  ServiceM D: service: logmanagersrv        pid:  70542
238  ServiceM D: Waiting exe: logmanagersrv        retryTimes = 1
183  ServiceM D: execl logmanagersrv (1).
192  ServiceM D: service: bindermanagersrv     pid:  70543
238  ServiceM D: Waiting exe: bindermanagersrv     retryTimes = 1
183  ServiceM D: execl bindermanagersrv (1).
192  ServiceM D: service: propertiessrv        pid:  70544
238  ServiceM D: Waiting exe: propertiessrv        retryTimes = 1
183  ServiceM D: execl propertiessrv (1).
192  ServiceM D: service: mediatorsrv          pid:  70545
183  ServiceM D: execl mediatorsrv (1).
238  ServiceM D: Waiting exe: mediatorsrv          retryTimes = 1
192  ServiceM D: service: sparrowsrv           pid:  70548
183  ServiceM D: execl sparrowsrv (1).
192  ServiceM D: service: powermanagersrv      pid:  70549
183  ServiceM D: execl powermanagersrv (1).
192  ServiceM D: service: debugsrv             pid:  70550
183  ServiceM D: execl debugsrv (1).
```

### 实时查看日志
```shell
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

### 内部命令集

项目集成了如下命令集，可通过`echo help > /tmp/{进程名}`查看具体进程支持命令集，回执在日志中：

* 查询 `statusmonitorsrv` 支持的命令
```Shell
$ echo help > /tmp/statusmonitorsrv
```

```C++
$ tail -f /tmp/sprlog/sparrow.log

 ==============================================================================
                      Debug Command List
 ==============================================================================
    1. Built-in Commands         Total:  2
       ├── help                : Dump all cmds
       ├── proc                : Dump process info
       └── version             : Dump version

    2. StatusMonitorManager      Total: 12
       ├── AddStatusEvent      : Add Status Event
       ├── DelAllEvent         : Del All Events
       ├── DelWithErrCode      : Del With ErrCode
       ├── DelWithID           : Del With ID
       ├── DelWithLevel        : Del With Level
       ├── DelWithText         : Del With Text
       ├── DisplayUTC          : Display UTC
       ├── DumpAllEvents       : Dump All Events
       ├── DumpWithErrCode     : Dump With ErrCode
       ├── DumpWithID          : Dump With ID
       ├── DumpWithLevel       : Dump With Level
       └── DumpWithText        : Dump With Text

 ==============================================================================
    E.g. echo help > /tmp/statusmonitorsrv
 ==============================================================================
```

* 查询 `servicemanagersrv` 支持的命令

```shell
$ echo help > /tmp/propertiessrv
```

```C++
$ tail -f /tmp/sprlog/sparrow.log

 ==============================================================================
                      Debug Command List
 ==============================================================================
    1. Built-in Commands         Total:  2
       ├── help                : Dump all cmds
       ├── proc                : Dump process info
       └── version             : Dump version

    2. Properties                Total:  1
       └── DumpAllProperties   : Dump all properties

 ==============================================================================
    E.g. echo help > /tmp/propertiessrv
 ==============================================================================
```

### 调试程序

项目同时集成了调试程序, 如下：
* infrawatch   \
实时查看各组件运行状态，需先启动`servicemanagersrv`拉起各个组件服务。
```
$ ./infrawatch
==================================  MAIN MENU  ==================================

    1. Display All Message Queues
    2. Manager's Entrance
    3. Custom Debug Options

    [Q] Quit

=================================================================================
```

* property_set、property_get    \
用于终端调试时设置和获取系统属性
```shell
$ ./property_get ro.SoftwareVersion
1.0.0
```

## 功能介绍
### 消息数据查看
### 事件监控
### 系统属性管理
### 一键退出
### 异常重启
### 崩溃堆栈捕获

## 第三方库依赖
|    第三方库   | 依赖模块     | 备注 |
| -----------  | -----------  | -- |
| sqlite       | ConfigManager| 用于项目运行中的关键配置等数据存储|
| googlettest  | Test         | 用于项目内部白盒测试、外部黑盒测试|

## 文档

项目进行过程中会输出一些文档，用于记录开发的一些细节、疑惑和值得记录的关键解决方案等。
```
Docs/
├── C++编程规范.md
├── Issue提交规范.md
├── Sparrow Q&A.md
├── TODO.md
├── png
├── share
├── spec
├── tips
├── 注意事项.md
├── 版本管理.md
├── 疑难杂症.md
└── 第三方依赖.md
```
=======
## 📄 许可
本项目使用 MIT 许可证，详见 [LICENSE](LICENSE)
>>>>>>> feature-versionmanager

---

## 💖 鼓励支持
如果您觉得本项目有意义，可以通过以下方式给予支持：
- 三连支持：Star、Fork、Watch
- 积极参与讨论：在Issues中提出问题或建议
- 公众号：开源519，关注、转发并点赞[公众号文章](https://mp.weixin.qq.com/s/DHiZ4iQJAQVrN8z4kDTiLg)
- 微信：StopCoding，加入技术交流群

<<<<<<< HEAD
## 最后
既入此行，砥砺精进；积硅步以至千里，聚小流而成江海。脚步不停，终达卓越！
=======
既入此行，砥砺精进；积硅步以至千里，聚小流而成江海。脚步不停，终达卓越！

>>>>>>> feature-versionmanager
