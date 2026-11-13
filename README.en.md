# Sparrow: Linux C++ Embedded Middleware

👉 [中文](README.md)

## 📌 What is it
`Sparrow` is a C++ middleware framework for `Linux / embedded device` environments, focused on `Epoll`-driven event processing, service-oriented communication, stable runtime operation, and issue diagnosis. It is suitable for building high-concurrency, loosely coupled, observable, and extensible device-side infrastructure.

## ✨ Key features

| Feature | Description |
| --- | --- |
| [Event-driven processing](Docs/01-Overview/功能概览.md) | Uses `Epoll` to handle I/O, timers, signals, and task events uniformly, making it suitable for high-concurrency device services. |
| [Service communication](Docs/02-Design/Components/Basics/MediatorSrv.md) | Provides `Binder`, message queues, and event dispatch capabilities to enable decoupling and coordination between modules. |
| [Service management](Docs/02-Design/Components/Basics/ServiceManagerSrv.md) | Manages service lifecycle, status, supervision, and recovery to improve system reliability. |
| [Configuration management](Docs/02-Design/Components/Basics/ConfigManagerSrv.md) | Centrally manages runtime parameters, configuration loading, and dynamic configuration capabilities. |
| [Property management](Docs/02-Design/Components/Basics/PropertiesSrv.md) | Unifies runtime state management, property updates, and state observation. |
| [Logging and debugging](Docs/02-Design/Components/Basics/LogManagerSrv.md) | Provides log output, debug nodes, and diagnostic entry points for rapid issue localization. |
| [Platform adaptation](Docs/01-Overview/系统架构.md) | Uses the `Hardware/` and `Platform/` layers to isolate platform differences and reduce porting costs. |
| [Test examples](TestCase/README.md) | Provides unit tests, benchmark tests, and example programs for engineering verification and extension. |

## 🚀 Quick start

### 1. Environment
- Linux / Ubuntu
- C++ 11 or later
- CMake / gcc / make

### 2. Build
```bash
cd Sparrow
cd Build
./rebuild_default.sh
```

### 3. Run
```bash
cd Release/Bin
./servicemanagersrv
```

### 4. View logs
```bash
tail -f /tmp/sprlog/sparrow.log
```

## 🧩 Project structure
```text
Sparrow/
├── Core/                  // Core framework modules
├── Components/            // Service components
├── PublicAPIs/            // Public interfaces
├── PrivateAPIs/           // Internal interfaces
├── Configs/               // Configuration files
├── Debug/                 // Debugging and monitoring
├── Docs/                  // Documentation
├── Examples/              // Examples
├── Hardware/              // Hardware adaptation
├── Platform/              // Platform differences
├── TestCase/              // Test cases
├── 3rdAdapter/            // Third-party adapter layer
├── 3rdParty/              // Third-party dependencies
├── Build/                 // Build entry point
├── Release/               // Output directory
├── CMakeLists.txt         // Top-level build entry point
├── README.md              // Main entry point
├── README.en.md           // English entry point
├── LICENSE                // License
└── version.cmake          // Version configuration
```

## 🧠 Suitable use cases
- **Industrial**: industrial gateways, edge gateways, data acquisition devices, industrial control hosts
- **Automotive**: TBOX, automotive controllers, body gateways, in-vehicle terminals
- **Security**: video surveillance terminals, access controllers, alarm hosts
- **Energy**: smart meters, power terminals, substation monitoring devices
- **Robotics**: robot controllers, edge computing nodes, smart devices
- **Consumer electronics**: TVs, smart speakers, watches, smart terminals, set-top boxes
- **Embedded Linux device platforms**: long-term stable operation, business extension, and status monitoring

## 🏗️ How to extend business code with Sparrow

It is recommended to scale the system by capability boundaries rather than placing business logic directly in `Core/`:

- `Components/`: place service components, business modules, and message-handling logic
- `PublicAPIs/`: expose public capability interfaces to upper-layer business code
- `PrivateAPIs/`: store internal protocols, events, and communication infrastructure
- `Core/`: keep general framework capabilities only to avoid business coupling
- `Hardware/` and `Platform/`: place platform-specific differences and hardware adaptation code

The most common extension pattern is:
1. Add a business service or task module under `Components/`
2. Expose the interface in `PublicAPIs/`
3. Communicate with other modules via Binder / MQ / Event
4. Integrate configuration, logging, and debugging capabilities in `Configs/` and `Debug/`

This keeps the core framework stable while centralizing business changes in the extension layer, making maintenance and reuse easier.

## 📚 Documentation index

- [Docs Navigation](Docs/README.md)
- [C++ Programming Standards](Docs/03-Standards/C++编程规范.md)
- [Issue Submission Guidelines](Docs/03-Standards/Issue提交规范.md)
- [Version Management](Docs/04-Development/版本管理.md)
- [Troubleshooting](Docs/06-Troubleshooting/疑难杂症.md)

## 📄 License
This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

---

## 💖 Support
If you find this project meaningful, you can support it in the following ways:
- Star, Fork, and Watch the project
- Take an active part in discussions and raise questions or suggestions in Issues
- Official account: Open Source 519; follow, share, and like the [official article](https://mp.weixin.qq.com/s/DHiZ4iQJAQVrN8z4kDTiLg)
- WeChat: StopCoding; join the technical communication group

As you continue on this path, keep striving to improve; step by step, you will eventually reach excellence.
