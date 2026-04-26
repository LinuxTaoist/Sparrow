# Sparrow: Linux C++ Embedded IoT Middleware
👉 [[中文]](README.md)

## 📌 Project Overview
`Sparrow` is a Linux C++ embedded IoT middleware framework based on the **epoll event-driven model**.
Adopting a **microservice & plug-in architecture** with full multi-platform adaptation capability, it delivers a high-performance, stable and extensible development foundation for smart devices, vehicle-mounted systems and IoT gateways.

## 🌟 Key Features
- **Out-of-the-box**: Core framework with minimal third-party dependencies, one-click compilation and rapid deployment
- **Multi-platform Support**: Natively adapts to Default platform, Quectel AG35 / AR590 and other modules with separated code & configuration
- **Service Orchestration**: Built-in system service cluster, supporting process daemon, status monitoring and auto-restart for high availability
- **Remote O&M**: Integrated `rshellx` for remote shell access and file transmission to accelerate field troubleshooting
- **Real-time Debugging**: Debug node, dynamic property access and log real-time viewing to improve development efficiency
- **Crash Tracing**: Automatic stack capture for program exceptions and hierarchical logging for fast issue locating
- **Code Quality Assurance**: Integrated cppcheck static analysis, Valgrind memory detection and GoogleTest unit testing
- **High-efficiency Communication**: Binder IPC RPC, message subscription & broadcast for low-coupling and high-concurrency module interaction
- **Ecosystem Extensibility**: Native SQLite / HTTP / MQTT protocol stack, supporting third-party adaptation and dynamic plugin loading

## 🚀 3 Minutes Quick Start
### 1. Environment Requirements
- C++11 or higher
- Linux System

### 2. Build Project
```bash
# Enter build directory and execute build script
$ cd Sparrow/Build
$ ./rebuild_default.sh

# All outputs are located in Release folder
$ cd Release
$ ls
Bin  Cache  Include  Lib
```

### 3. Start Core Services
```bash
# Enter binary directory, launch service manager to auto start core services
$ cd Release/Bin
$ ./servicemanagersrv
```

### 4. Real-time Log Viewing
```bash
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

## 📢 Function Introduction
### Core Functions
| Function | Key Features | Description |
|---------|-------------|-------------|
| Process Management | Process launching, daemon, status monitor & auto restart | Ensure long-term stable operation of core services |
| Log Management | Graded log, dynamic level adjustment, formatting & persistence | Standardized log system for efficient troubleshooting |
| Binder RPC | Sync/async communication, service registration & discovery | High-performance cross-process communication core |
| Message Dispatch | Message broadcast, subscribe-publish & message queue | Decouple modules for asynchronous interaction |
| Property Service | Property read/write, change notification & configuration persistence | Unified system configuration management |
| Power Management | Power state control, event notification & low-power adaptation | Full lifecycle power management for embedded devices |
| Plugin Management | Dynamic loading/unloading and modular extension | Flexible feature expansion with low coupling |
| Timer Scheduler | Scheduled tasks, periodic tasks and callback trigger | High-precision timing task management |
| Status Monitor | Timeout detection, device status collection & exception recording | Full-link runtime status monitoring |
| Thread Pool | Thread reuse, async task scheduling & callback | High-concurrency task processing |
| Event-driven Core | Epoll multiplexing for IO/timer/signal unified scheduling | Low-power & high-concurrency runtime foundation |
| Debug Node | Custom debug entry registration, trigger and response | Online simulation and debugging without reboot |
| Crash Capture | Automatic exception stack recording | Rapid crash root cause analysis |
| Unit Test | GoogleTest integration for core module verification | Reduce regression risks and ensure code quality |
| Static Analysis | Cppcheck code scan & Valgrind memory leak detection | Early warning of potential code defects |
| Multi-platform Migration | Isolated platform config & cross-compilation switching | Rapid porting for diverse hardware |

### Auxiliary Tools
| Tool | Key Features | Description |
|------|-------------|-------------|
| rshellx | Remote shell execution & file transmission | Remote maintenance for headless embedded devices |
| infrawatch | Service monitoring, real-time log & interactive debug | One-stop online diagnosis for embedded devices |
| logshow | Log filtering and real-time viewing | Lightweight log analysis tool |
| getprop/setprop | Fast system property read/write | Dynamic configuration debugging |

## 🎯 Application Scenarios
- Smart Hardware: IP cameras, robot control systems, industrial sensors and intelligent terminals
- Vehicle-mounted System: T-Box, intelligent cockpit and vehicle networking gateway
- IoT Gateway: Smart home hub, environmental monitoring gateway and industrial IoT data forwarder
- Consumer Electronics: Wearable devices, health monitors and smart home appliances

## 📂 Source Code Structure
```
Sparrow/
├── 3rdAdapter                // Third-party library adaptation layer (SQLite / Log wrapper)
├── 3rdParty                  // Third-party dependencies (gtest / libgo / sqlite)
├── Build                     // Entry of build & rebuild scripts
├── CMakeLists.txt            // Top-level project CMake config
├── Components                // Business & system service components
│   ├── Basics                // Core system services (Binder / Log / Power / Property)
│   └── Business              // Business components (OneNet MQTT)
├── Configs                   // Global system configuration files
├── Core                      // Framework core (Binder / Epoll / ThreadPool / Message)
├── Debug                     // Debug toolset (infrawatch / rshellx / logshow)
├── Docs                      // Official documents & specifications
├── Examples                  // Practical demo codes for API usage
├── Hardware                  // Hardware adaptation interface
├── LICENSE                   // Open-source license file
├── Platform                  // Multi-platform adaptation config (Default / AG35 / AR590)
├── PrivateAPIs               // Internal framework private interfaces
├── PublicAPIs                // External open business interfaces
├── README.en.md
├── README.md
├── Release                   // Build output (Bin / Lib / Include / Cache)
├── TestCase                  // Unit test & integration test cases
├── Tools                     // Quality inspection tools (cppcheck / valgrind)
├── Util                      // General utilities (JSON / Backtrace / Algorithm)
├── UtilModules               // Common middleware modules (Codec / HTTP / MQTT)
├── modules_config.cmake      // Global module compilation config
└── version.cmake             // Project version management
```

## 📚 Documents & Resources
- [User Manual](Docs/UserManual): Guidelines for debug node, timer, log, message dispatch and debugging functions.
- [C++ Coding Specification](Docs/C++编程规范.md): Unified code style and project development standards.
- [Version Management](Docs/版本管理.md): Iteration records and release notes.
- [FAQ](Docs/疑难杂症.md): Collection of common problems and solutions.

## 🤝 Contribute & Contact
- Issues: Submit bugs, feature requests or technical discussions.
- Official Account: 开源519
- WeChat: StopCoding (Note: Sparrow)

## 📄 License
This project is licensed under the **MIT License**, see the [LICENSE](LICENSE) file for details.

---

## 💖 Support
If this project helps you, please support us:
- Star, Fork and Watch this repository
- Participate in discussions via Issues
- Follow and share our official technical articles
- Join the technical communication group via WeChat

Keep improving in embedded development; every step leads to long-term progress.
