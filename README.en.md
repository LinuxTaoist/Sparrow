# Sparrow: Linux C++ Embedded Middleware
👉 [[中文]](README.md)

## 📌 Project Positioning
`Sparrow` is an `Linux` middleware framework based on the event-driven model. It adopts a microservice architecture and aims to quickly build efficient `Linux C++` middleware application frameworks.


## 🌟 Core Highlights
- **Out-of-the-box**: Basic functions do not depend on third-party libraries, enabling out-of-the-box use and rapid deployment.
- **Remote Troubleshooting**: Provides the remote tool `rshellx` to implement remote `shell` execution and quickly locate problems.
- **Configuration Separation**: Separates configuration files from code to easily achieve project differentiation.
- **Status Monitoring**: Offers status monitoring function to record various priority events such as interface execution timeout and device status change.
- **Static Scanning**: Provides tools for static scanning of `shell` scripts, and cooperates with `cppcheck` to realize code risk scanning.
- **Real-time Debugging**: Integrates debugging node function to facilitate triggering simulation and debugging during the development phase.
- **Crash Capture**: Automatically records crash stacks when the program crashes abnormally, facilitating problem troubleshooting.
- **Unit Testing**: Provides test cases and cooperates with `googletest` to facilitate full-function verification after code modification.
- **Plug-in Programming**: Supports plug-in programming to realize dynamic loading and unloading of functional modules.

For more functions, please refer to the Function Introduction.

## 🚀 Quick Start in 3 Minutes
### 1. Environment Requirements
- C++11 or higher version
- Linux system

### 2. Compile the Project
```bash
# Enter the build directory and execute the script
$ cd Sparrow/Build
$ ./rebuild_general.sh

# Compiled products are in the Release directory
$ cd Release
$ ls
Bin  Cache  Include  Lib
```

### 3. Start the Service
```bash
# Enter the binary directory and start the service manager (automatically starts all core services)
$ cd Release/Bin
$ ./servicemanagersrv
```

### 4. View Logs in Real Time
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

## 📢 Function Introduction

### Core Functions
| Function          | Key Features                                  | Description                          |
|-------------------|-----------------------------------------------|--------------------------------------|
| Process Management | Process launching, process guarding, process status monitoring | Ensure stable operation of key services |
| Log Management    | Dynamic adjustment of log levels, formatted log output | Facilitate problem location and debugging |
| RPC Communication | Support for synchronous and asynchronous communication |                                      |
| Message Transfer  | Support for message broadcasting and message subscription | Realize decoupling between modules    |
| Property Management | Support for property reading/writing, property change notification, property export | Realize configuration management     |
| Power Management  | Support for power status management and power event notification | Realize device power management      |
| Plug-in Management | Support for dynamic loading and unloading of plug-ins | Realize dynamic expansion of functional modules |
| Timer Management  | Support for scheduled tasks and scheduled task callbacks | Realize scheduled task management    |
| Status Monitoring | Support for status exception capture and storage |                                      |
| Thread Pool       | Support for thread pool creation, task submission, and task callbacks |                                      |
| Debug Node        | Support for registration, cancellation, and trigger response of debugging entrances for each module | Facilitate debugging                 |
| Test Case         | Cooperate with gtest to facilitate full-function verification after code modification | Improve code quality and reduce risks |
| Crash Capture     | Automatically record crash stacks when the program crashes abnormally, facilitating problem troubleshooting | Improve code quality and reduce risks |
| Static Scanning   | Static code scanning and code risk analysis   | Improve code quality and reduce risks |

### Auxiliary Tools
| Tool          | Key Features                                  | Description                          |
|---------------|-----------------------------------------------|--------------------------------------|
| rshellx       | Remote shell execution, remote file transfer  | Quickly locate problems in hardware scenarios |
| infrawatch    | Status monitoring, real-time logging, remote debugging | Locate problems online without restarting the device |


## 🎯 Application Scenarios
- Smart Hardware: Such as smart light bulbs, smart sockets, smart cameras, robot control systems, etc., suitable for devices that require efficient communication and control.
- Consumer Electronics: For example, smart watches, health monitoring bracelets, smart TVs, etc., supporting data collection, analysis, and user interface management.
- In-Vehicle Systems: Including in-vehicle T-Box (Telematics Box), smart cockpits, etc., providing navigation, entertainment, phone connection and other functions to enhance the driving experience.
- Internet of Things (IoT): Covering smart homes, environmental monitoring, IoT gateways, etc., realizing device interconnection, data collection and processing.


## 📂 Source Code Structure Overview
```
Sparrow/
├── 3rdAdapter                // Third-party software adaptation interface
├── 3rdParty                  // Third-party software library
├── Build                     // Build scripts
├── CMakeLists.txt
├── Components                // Independent components
├── Core                      // Core framework
├── Debug                     // Debugging tools
├── Docs                      // Documents
├── Examples                  // Examples
├── Hardware                  // Hardware adaptation interface
├── LICENSE                   // License
├── PrivateAPIs               // Internal public interfaces
├── ProjectConfigs            // Project configurations
├── PublicAPIs                // External public interfaces
├── README.en.md
├── README.md
├── Release                   // Release path
├── TestCase                  // Test cases
├── Tools                     // Common auxiliary scripts
├── Util                      // General tool functions
├── UtilModules               // General module library
├── modules_configs.cmake     // Module configuration
└── version.cmake             // Version information
```


## 📚 Documents and Resources
- [User Guide](Docs/UserManual): Contains usage instructions for functions such as debug nodes, timers, and logs.
- [Programming Specifications](Docs/C++ Programming Specification.md): Unify the code style, with priority given to the current project code.
- [Version Management](Docs/Version Management.md): Version release records, continuously updated.
- [FAQs & Troubleshooting](Docs/FAQs & Troubleshooting.md): Solutions to common problems, continuously updated.


## 🤝 Interaction and Communication
- Issues: Submit bugs, suggestions or participate in discussions.
- WeChat Official Account: KaiYuan519
- WeChat: StopCoding (please note "Sparrow" when adding)


## 📄 License
This project is licensed under the MIT License. For details, see [LICENSE](LICENSE)

---

## 💖 Support and Encouragement
If you find this project meaningful, you can support it in the following ways:
- Threefold Support: Star, Fork, Watch
- Actively participate in discussions: Raise questions or suggestions in Issues
- WeChat Official Account: Follow "KaiYuan519", forward and like the [official account article](https://mp.weixin.qq.com/s/DHiZ4iQJAQVrN8z4kDTiLg)
- WeChat: Add "StopCoding" to join the technical exchange group

Once in this field, we strive for progress; every small step leads to a thousand miles, and every small stream merges into a river. Keep moving forward, and we will eventually achieve excellence!
