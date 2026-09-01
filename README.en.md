# Sparrow: Linux C++ Embedded Middleware

👉 [中文](README.md)

## 📌 Project Positioning
`Sparrow` is a C++ middleware framework for `Linux / embedded device` scenarios. Built on `Epoll` event-driven design and service-oriented architecture, it is suitable for building high-concurrency, low-coupling, observable, and extensible device-system infrastructure.

It is guided by engineering practice and focuses on:

- High-performance event scheduling
- Modular service communication
- Stable operation and diagnostics
- Platform / hardware adaptation capability
- Fast onboarding and continuous evolution for developers

## ✨ Why It Is Worth Paying Attention To

- **Event-driven engine**<br>Handles IO, timers, thread tasks, and signal events in one unified model, suitable for high-concurrency device middleware scenarios
- **Service-oriented architecture**<br>Uses Binder / MQ / event dispatch mechanisms to keep modules loosely coupled and easier to extend
- **Strong observability**<br>Logs, properties, debug nodes, and status monitoring can be used directly for on-site troubleshooting
- **Stable runtime support**<br>Process supervision, status collection, anomaly observation, and restart recovery support long-term stable runtime on devices
- **Engineering-friendly design**<br>The directory structure is clear, and examples, tests, documentation, and platform adaptation capabilities are relatively complete, making it easy to get started quickly

## 🚀 Quick Start in 3 Minutes

### 1. Requirements
- Linux / Ubuntu
- C++11 or higher
- CMake / gcc / make

### 2. Build the project
```bash
cd /path/to/Sparrow
cd Build
./rebuild_default.sh
```

Build artifacts are usually generated under:
```bash
cd Release
ls
```

Common directories:
- `Bin`: executable files
- `Lib`: static / dynamic libraries
- `Include`: exported headers
- `Cache`: build and runtime cache

### 3. Start the service
```bash
cd Release/Bin
./servicemanagersrv
```

### 4. View logs
```bash
tail -f /tmp/sprlog/sparrow.log
```

## 📢 Core Capabilities

| Function | Key Features | Value |
|---|---|---|
| Process Management | Supervision, restart, status monitoring, exception recovery | Ensures stable operation of core services |
| Log Management | Hierarchical logging, formatted output, persistence | Speeds up issue localization and reduces troubleshooting cost |
| Binder / RPC | Service registration, message forwarding, synchronous / asynchronous calls | Supports module collaboration and inter-process communication |
| Message Relay | Broadcast, subscription, MQ event dispatch | Reduces coupling and improves extensibility |
| Property Management | Dynamic read/write, change notifications, configuration export | Makes system configuration more flexible and easier to tune |
| Debug Node | Dynamic debug entry, status observation, online diagnosis | Reduces reboot and secondary development costs |
| Timers and Thread Pool | Periodic tasks, async task submission, callback triggering | Improves concurrency and task processing efficiency |
| Multi-platform Adaptation | Layered adaptation, platform isolation, cross-device support | Facilitates porting to different hardware and platforms |

## 🏗️ Main Repository Structure Overview
```text
Sparrow/
├── 3rdAdapter/            // Third-party adapter layer
├── 3rdParty/              // Third-party dependencies
├── Build/                 // Build script entry
├── CMakeLists.txt         // Top-level build entry
├── Components/            // Service components
├── Configs/               // Configuration files
├── Core/                  // Core framework modules
├── Debug/                 // Debugging and monitoring tools
├── Docs/                  // Engineering documentation
├── Examples/              // Example programs
├── Hardware/              // Hardware adaptation layer
├── Platform/              // Platform configuration layer
├── PrivateAPIs/           // Internal interfaces
├── PublicAPIs/            // Public interfaces
├── Release/               // Build output directory
├── TestCase/              // Test cases
├── Tools/                 // Quality tools
├── Util/                  // Utility libraries
├── UtilModules/           // Common functional modules
├── LICENSE                // Open-source license
├── README.md              // Main repository documentation
├── README.en.md           // English documentation
├── modules_config.cmake   // Module configuration
├── version.cmake          // Version configuration
└── ...
```

## 🔌 If You Need to Extend It, Where Should You Put It?

The extension entry points are clearly layered. It is recommended to place code according to the boundary of responsibilities:

- **Business feature extension** <br>`Components/Business/`, `PublicAPIs/`
  Suitable for customer-specific business logic, external capability encapsulation, and business service modules
- **Platform-specific configuration** <br>`Platform/` and `Configs/`
  Suitable for platform build parameters, environment differences, and configuration files
- **Hardware adaptation** <br>`Hardware/`
  Suitable for drivers, board abstraction, and HAL-layer implementations
- **General utility capabilities** <br>`Util/`, `UtilModules/`
  Suitable for reusable utilities, protocol handling, algorithms, and foundational components
- **Keep the main repository stable** <br>`Core/`
  Only the most common framework capabilities should stay here; avoid putting customer-specific or platform-specific logic directly into the core layer

> In other words: the main repository is responsible for “common infrastructure,” while the extension layer handles “business, platform, and hardware differences.” This is the easiest to maintain and matches the current project structure best.

## 📚 Documentation Resources
- [C++ Programming Guide](Docs/C++编程规范.md): Unifies code style and keeps the project consistent
- [Issue Submission Guidelines](Docs/Issue提交规范.md): Standardizes collaboration and quality requirements
- [Version Management](Docs/版本管理.md): Explains releases and maintenance
- [Troubleshooting](Docs/疑难杂症.md): Covers common issues, fault handling, and experience accumulation

## 🎯 Applicable Scenarios
- Smart hardware: cameras, sensors, terminals, controllers
- In-vehicle systems: T-Box, vehicle gateways, intelligent cockpits
- Industrial and edge scenarios: field control, edge gateways, data acquisition nodes
- General embedded Linux middleware: systems that require stability, extensibility, and observability

## 🤝 Who Is It For?
- Engineering teams building Linux embedded middleware
- Device software developers who want unified logging, status, and debugging capabilities
- R&D personnel who need strong extensibility and platform adaptation capability

## 📄 License
This project is licensed under the MIT License. See [LICENSE](LICENSE).

---

## 💎 One-Line Summary
If you need a Linux C++ device middleware foundation that is easy to get started with, easy to extend, and convenient for stable operation and troubleshooting, `Sparrow` is the main repository entry worth paying attention to.
