# Sparrow：Linux C++ 嵌入式中间件

👉 [English](README.en.md)

## 📌 这是什么
`Sparrow` 是一个面向 `Linux / 嵌入式设备` 场景的 C++ 中间件框架，聚焦 `Epoll` 事件驱动、服务化通信、稳定运行与问题排查，适合构建高并发、低耦合、可观测、可扩展的设备端基础设施。

## ✨ 功能亮点

Sparrow 当前提供的核心能力，重点体现在“稳定运行 + 模块通信 + 可观测性”三类能力：

- **事件驱动框架**：基于 `Epoll` 统一处理 IO、定时器、线程任务和信号事件，适合高并发设备服务场景
- **模块化通信机制**：提供 `Binder`、消息队列、事件分发等能力，便于服务之间解耦和协作
- **进程与服务管理**：支持守护、状态管理、异常恢复、进程信息采集，提升系统稳定性
- **配置与属性管理**：统一管理运行参数、属性配置和动态状态，方便调试和迭代
- **日志与调试能力**：日志、调试节点、状态展示和诊断入口，便于快速定位问题
- **平台与硬件适配**：通过 `Hardware/` 与 `Platform/` 分层处理平台差异，降低移植成本
- **工程实践支持**：包含示例、测试、文档和构建脚本，便于直接上手和扩展业务

一句话理解：Sparrow 是面向 Linux / 嵌入式设备的基础中间件和服务化运行框架。

## 🚀 快速开始

### 1. 环境
- Linux / Ubuntu
- C++ 11及以上
- CMake / gcc / make

### 2. 编译
```bash
cd Sparrow
cd Build
./rebuild_default.sh
```

### 3. 运行
```bash
cd Release/Bin
./servicemanagersrv
```

### 4. 查看日志
```bash
tail -f /tmp/sprlog/sparrow.log
```

## 🧩 关键目录
```text
Sparrow/
├── Core/                  // 核心框架模块
├── Components/            // 服务组件
├── PublicAPIs/            // 对外接口
├── PrivateAPIs/           // 内部接口
├── Configs/               // 配置文件
├── Debug/                 // 调试与监控
├── Docs/                  // 文档
├── Examples/              // 示例
├── Hardware/              // 硬件适配
├── Platform/              // 平台差异
├── TestCase/              // 测试用例
├── 3rdAdapter/            // 第三方适配层
├── 3rdParty/              // 第三方依赖
├── Build/                 // 构建入口
├── Release/               // 输出目录
├── CMakeLists.txt         // 顶层构建入口
├── README.md              // 主入口
├── README.en.md           // 英文入口
├── LICENSE                // 许可证
└── version.cmake          // 版本配置
```

## 🧠 适合什么场景
- **工业**：工业网关、边缘网关、数据采集器、工控主机
- **车载**：TBOX、车载控制器、车身网关、车载终端
- **安防**：视频监控终端、门禁控制器、报警主机
- **能源**：智能电表、用电终端、变电站监测设备
- **机器人**：机器人控制器、边缘计算节点、智能设备
- **消费电子**：TV、智能音箱、手表、智能终端、盒子设备
- **嵌入式 Linux 设备平台**：长期稳定运行、业务扩展、状态监控

## 🏗️ 如何基于 Sparrow 扩展业务代码

推荐按“能力边界”扩展，不要把业务逻辑直接塞进 `Core/`：

- `Components/`：放服务组件、业务模块、消息处理逻辑
- `PublicAPIs/`：放对外能力接口，供上层业务使用
- `PrivateAPIs/`：放内部协议、事件、通信基础能力
- `Core/`：保留通用框架能力，避免业务耦合
- `Hardware/`、`Platform/`：放平台差异和硬件适配

最常见的扩展方式：
1. 在 `Components/` 中新增业务服务或任务模块
2. 在 `PublicAPIs/` 中暴露接口
3. 通过 Binder / MQ / Event 方式与其他模块通信
4. 在 `Configs/` 和 `Debug/` 中接入配置、日志和调试能力

这样可以保持核心框架稳定，业务变更集中在扩展层，便于维护和复用。

## 📚 文档入口

- [Docs 导航](Docs/README.md)
- [C++ 编程规范](Docs/03-Standards/C++编程规范.md)
- [Issue 提交规范](Docs/03-Standards/Issue提交规范.md)
- [版本管理](Docs/04-Development/版本管理.md)
- [疑难杂症](Docs/06-Troubleshooting/疑难杂症.md)

## 📄 许可证
本项目采用 MIT 许可证，详情见 [LICENSE](LICENSE)。

---

## 💎 一句话总结
如果你要的是一个适合 Linux / 嵌入式设备场景的 C++ 基础中间件，`Sparrow` 提供的是稳定运行、模块化扩展、日志与调试能力齐全的工程化基础。