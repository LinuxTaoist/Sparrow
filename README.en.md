# Sparrow: Linux C++ Embedded Middleware

👉 [[中文]](README.md)

## 📌 Project Positioning
`Sparrow` is a `Linux C++` embedded middleware framework based on the **epoll event-driven model**. It uses a **microservice + plugin architecture**, supports seamless migration across multiple hardware platforms, and provides an efficient, stable, and extensible foundation for scenarios such as smart devices and in-vehicle systems.

## 🌟 Core Highlights
- **Out of the box**: The core framework has no hard third-party dependency. It supports one-click multi-platform build for fast deployment.
- **Easy multi-platform migration**: Code and configuration are separated. Embedded Linux platforms can be adapted quickly with lower migration cost.
- **High service availability**: Built-in service cluster supports process guarding, status monitoring, and automatic restart on exceptions.
- **Lightweight remote O&M**: Integrated `rshellx` supports remote `Shell` capabilities for remote troubleshooting.
- **Web visual debugging**: Built-in `huisrv` lightweight HTTP UI addresses the common pain point of no GUI and difficult middleware debugging.
- **Generic protocol codec**: Built-in `CodecX` can adapt multiple protocol parsing requirements via configuration, avoiding repetitive development.
- **One-stop real-time debugging**: Supports debug nodes, dynamic property read/write, and real-time log viewing to improve debugging efficiency end to end.
- **Automatic exception tracing**: Program crashes are automatically captured with stack logs. Combined with hierarchical logs, issues can be located quickly.
- **R&D quality assurance**: Integrates `cppcheck` static analysis and `googletest` unit tests for strict code quality control.
- **Plugin-based design**: Supports dynamic loading and unloading of shared libraries.

## 🚀 Quick Start in 3 Minutes
### 1. Requirements
- C++11 or later
- Linux

### 2. Build the project
```bash
# Go to the build directory and run the script
$ cd Sparrow/Build
$ ./rebuild_default.sh

# Build outputs are in the Release directory
$ cd Release
$ ls
Bin  Cache  Include  Lib
```

### 3. Start services
```bash
# Go to binary directory and start service manager
# (it launches all core services automatically)
$ cd Release/Bin
$ ./servicemanagersrv
```

### 4. View logs in real time
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

## 📢 Feature Overview
### Core Features
| Feature | Key Capabilities | Description |
|---------------|-----------------------------------------|----------------------------|
| Process Management | Process launch, guarding, status monitoring, exception restart | Ensures stable 24x7 operation of core services |
| Log Management | Hierarchical logs, dynamic level adjustment, formatted output, persistence | Standardized log system for efficient troubleshooting |
| Binder RPC | Sync/async communication, service registration and discovery | High-performance cross-process communication core |
| Message Relay | Message broadcast, publish-subscribe, message queue | Decoupled modules and asynchronous communication |
| Property Management | Property read/write, change notification, config export, persistence | Unified configuration management center |
| Power Management | Power state control, event notification, low-power adaptation | Power lifecycle management for embedded devices |
| Plugin Management | Dynamic load/unload, modular extension | Flexible feature plug-in and lower coupling |
| Timer Management | Scheduled tasks, periodic tasks, callback triggering | High-precision task scheduling |
| Status Monitoring | API timeout, device status, exception event collection and storage | Full-dimensional runtime status monitoring |
| Thread Pool | Thread reuse, async task submission, callback notification | High-concurrency task processing |
| Event-driven Core | Epoll event loop, unified management of IO/timer/signal events | Core for high concurrency and low power |
| Debug Node | Debug entry registration/trigger/response | Online simulation debugging without device reboot |
| Crash Capture | Automatic exception stack recording | Fast root-cause location for crashes |
| Automated Testing | Integrated gtest, unit/integration tests for core modules | Ensures code quality and reduces change risk |
| Static Analysis | cppcheck risk detection, valgrind memory detection | Prevents defects early |
| Multi-platform Adaptation | Isolated platform config, one-click cross-build switch | Fast migration to different hardware platforms |

### Auxiliary Tools
| Tool | Key Capabilities | Description |
|---------------|-----------------------------------------|----------------------------|
| rshellx | Remote shell execution | Remote O&M for devices without displays |
| huisrv | Lightweight Web UI, supports device data view and shell execution | Online device data presentation |
| infrawatch | Service status monitoring, real-time logs, remote debug interaction | One-stop online diagnostics |
| logshow | Real-time log viewing, filtering and search | Lightweight log viewer |
| getprop/setprop | Fast system property read/write | Dynamic configuration debugging |

## 🎯 Scenarios
- Smart hardware: smart cameras, robot control systems, industrial sensors, smart terminals, and other embedded devices
- In-vehicle systems: T-Box, intelligent cockpit, vehicle networking gateways, and other automotive terminals
- Consumer electronics: smart wearables, health monitors, smart home appliances, and other lightweight smart products

## 📂 Source Tree at a Glance
```
Sparrow/
├── 3rdAdapter                // Third-party library adaptation layer (SQLite/log wrapper)
├── 3rdParty                  // Third-party dependencies (gtest/libgo/sqlite)
├── Build                     // Build script entry
├── CMakeLists.txt            // Top-level build config
├── Components                // Service component set
│   ├── Basics                // Core system services (Binder/log/power/property, etc.)
│   └── Business              // Business components (OneNet MQTT cloud integration)
├── Configs                   // Global config files
├── Core                      // Framework core (Binder/event scheduler/thread pool/message mechanism)
├── Debug                     // Debug tools (infrawatch/rshellx/logshow)
├── Docs                      // Project docs (specs/guides/architecture/troubleshooting)
├── Examples                  // Feature usage examples
├── Hardware                  // Hardware adaptation interfaces
├── LICENSE                   // Open-source license
├── Platform                  // Multi-platform adaptation config
├── PrivateAPIs               // Internal framework APIs (Binder/debug/mediator)
├── PublicAPIs                // Public business APIs (property/power/OneNet)
├── README.en.md
├── README.md
├── Release                   // Build outputs (binaries/libraries/headers/cache)
├── TestCase                  // Test cases (internal core/external extensions)
├── Tools                     // Quality tools (cppcheck/valgrind)
├── Util                      // Utility library (serialization/JSON/backtrace/algorithms)
├── UtilModules               // Base modules (codec/epoll/HTTP/MQTT)
├── modules_config.cmake      // Module build config
└── version.cmake             // Version config
```

## 📚 Docs and Resources
- [User Manual](Docs/UserManual): Usage guide for debug node, timer, log, and other features.
- [Coding Specification](Docs/C++编程规范.md): Unified coding style. Follow the current project code first.
- [Version Management](Docs/版本管理.md): Version release records, continuously updated.
- [Troubleshooting](Docs/疑难杂症.md): Common issue solutions, continuously updated.

## 🤝 Community
- Issues: Submit bugs, suggestions, or join discussions.
- WeChat Official Account: 开源519
- WeChat: StopCoding (note: Sparrow)

## 📄 License
This project is licensed under the MIT License. See [LICENSE](LICENSE).

---

## 💖 Support
If you find this project useful, you can support it in the following ways:
- Triple support: Star, Fork, Watch
- Join discussions: raise issues or suggestions in Issues
- WeChat Official Account: 开源519, follow/share/like [official article](https://mp.weixin.qq.com/s/DHiZ4iQJAQVrN8z4kDTiLg)
- WeChat: StopCoding, join the technical discussion group

Once committed to this pursuit, hone your craft unremittingly.Progress stems from incremental steps, greatness from steady accumulation Persist, and excellence shall be attained.
