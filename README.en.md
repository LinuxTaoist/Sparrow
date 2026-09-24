# Sparrow: Linux C++ Embedded Middleware

👉 [中文](README.md)

## 📌 What is this
`Sparrow` is a C++ middleware framework for `Linux / embedded devices`, focusing on `Epoll` event-driven processing, service-oriented communication, stable operation, and issue diagnosis. It is suitable for building high-concurrency, loosely coupled, observable, and extensible device-side infrastructure.

## ✨ Key features

| Feature | Description |
| --- | --- |
| [Event-driven processing](Docs/01-Overview/功能概览.md) | Uses `Epoll` to uniformly handle IO, timers, signals, and task events for high-concurrency device services. |
| [Service communication](Docs/02-Design/Components/Basics/MediatorSrv.md) | Provides `Binder`, message queues, and event dispatch for decoupled coordination between modules. |
| [Service management](Docs/02-Design/Components/Basics/ServiceManagerSrv.md) | Manages service lifecycles, status, supervision, and recovery to improve system stability. |
| [Configuration management](Docs/02-Design/Components/Basics/ConfigManagerSrv.md) | Centrally manages runtime parameters, configuration loading, and dynamic configuration. |
| [Property management](Docs/02-Design/Components/Basics/PropertiesSrv.md) | Centrally manages runtime status, property updates, and status observation. |
| [Logging and debugging](Docs/02-Design/Components/Basics/LogManagerSrv.md) | Provides log output, debug nodes, and diagnostic entry points for rapid issue diagnosis. |
| [Platform adaptation](Docs/01-Overview/系统架构.md) | Uses the `Hardware/` and `Platform/` layers to isolate platform differences and reduce porting costs. |
| [Tests and examples](TestCase/README.md) | Provides unit tests, benchmark tests, and example programs for engineering verification and extension. |

## 🚀 Quick start

### 1. Requirements
- Linux / Ubuntu
- C++11 or later
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

## 🧩 Project directories
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

## 🧠 Suitable scenarios
- **Industrial**: industrial gateways, edge gateways, data collectors, industrial control hosts
- **Automotive**: TBOX, vehicle controllers, body gateways, in-vehicle terminals
- **Security**: video surveillance terminals, access controllers, alarm hosts
- **Energy**: smart meters, power terminals, substation monitoring devices
- **Robotics**: robot controllers, edge computing nodes, smart devices
- **Consumer electronics**: TVs, smart speakers, watches, smart terminals, set-top boxes
- **Embedded Linux device platforms**: long-term stable operation, business extension, and status monitoring

## 🏗️ How to extend business code based on Sparrow

It is recommended to extend the project by capability boundaries rather than putting business logic directly into `Core/`:

- `Components/`: service components, business modules, and message-processing logic
- `PublicAPIs/`: public capability interfaces for upper-layer business code
- `PrivateAPIs/`: internal protocols, events, and communication infrastructure
- `Core/`: common framework capabilities only; avoid business coupling
- `Hardware/` and `Platform/`: platform differences and hardware adaptation

The most common extension flow is:
1. Add a business service or task module under `Components/`
2. Expose its interface in `PublicAPIs/`
3. Communicate with other modules through Binder / MQ / Event
4. Integrate configuration, logging, and debugging through `Configs/` and `Debug/`

This keeps the core framework stable, concentrates business changes in the extension layer, and facilitates maintenance and reuse.

## 📚 Documentation

- [Docs Index](Docs/README.md)
- [C++ Programming Guidelines](Docs/03-Standards/C++编程规范.md)
- [Issue Submission Guidelines](Docs/03-Standards/Issue提交规范.md)
- [Version Management](Docs/04-Development/版本管理.md)
- [Troubleshooting](Docs/06-Troubleshooting/疑难杂症.md)

## 📄 License
This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

---

## 💎 One-line summary
If you need C++ foundation middleware for Linux / embedded device scenarios, `Sparrow` provides an engineering foundation with stable operation, modular extension, and comprehensive logging and debugging capabilities.
# Sparrow: Linux C++ Embedded Middleware

👉 [中文](README.md)

## 📌 What is this
`Sparrow` is a C++ middleware framework for `Linux / embedded device` scenarios. It focuses on `Epoll` event-driven architecture, service-oriented communication, stable runtime behavior, and issue diagnosis.

## ✨ Core capabilities

The current value of Sparrow is centered on three practical areas: stable runtime, modular communication, and observability:

- **Event-driven framework**: `Epoll`-based processing for IO, timers, thread tasks, and signals, suitable for high-concurrency device services
- **Modular communication**: `Binder`, message queues, and event dispatch help decouple and coordinate service modules
- **Process and service management**: support for supervision, runtime state tracking, recovery, and process info collection
- **Configuration and property management**: unified runtime parameters and property handling for easier debugging and iteration
- **Logging and debugging**: logs, debug nodes, state display, and diagnostic entry points make issues easier to locate
- **Platform and hardware adaptation**: `Hardware/` and `Platform/` isolate platform differences and reduce porting overhead
- **Engineering support**: examples, tests, documents, and build scripts make the project easier to adopt and extend

In one sentence: Sparrow is not just a library; it is a Linux / embedded foundation middleware and service runtime for practical device software.

## 🚀 Quick start

### 1. Requirements
- Linux / Ubuntu
- C++11+
- CMake / gcc / make

### 2. Build
```bash
cd /path/to/Sparrow
cd Build
./rebuild_default.sh
```

### 3. Run
```bash
cd Release/Bin
./servicemanagersrv
```

### 4. Read logs
```bash
tail -f /tmp/sprlog/sparrow.log
```

## 🧩 Key directories
```text
Sparrow/
├── Core/                  // Core framework modules
├── Components/            // Service components
├── PublicAPIs/            // Public interfaces
├── PrivateAPIs/           // Internal interfaces
├── Configs/               // Configuration
├── Debug/                 // Debug and monitoring tools
├── Docs/                  // Documentation
├── Examples/              // Examples
├── Hardware/              // Hardware adaptation
├── Platform/              // Platform differences
├── TestCase/              // Tests
├── 3rdAdapter/            // Third-party adapter layer
├── 3rdParty/              // Third-party dependencies
├── Build/                 // Build entry
├── Release/               // Build output
├── CMakeLists.txt         // Top-level build entry
├── README.md              // Main entry
├── README.en.md           // English entry
├── LICENSE                // License
├── version.cmake          // Version metadata
└── ...
```

## 🧠 Best fit

- **Industrial**: industrial gateways, edge gateways, data collectors, industrial control hosts
- **Automotive**: TBOX, vehicle controllers, in-vehicle gateways, automotive terminals
- **Security**: video monitoring terminals, access controllers, alarm hosts
- **Energy**: smart meters, power terminals, substation monitoring devices
- **Robotics**: robot controllers, edge computing nodes, smart devices
- **Consumer electronics**: TVs, smart speakers, smartwatches, terminals, set-top devices
- **Embedded Linux device platforms**: long-term stable operation, business extension, and status monitoring

`Sparrow` is primarily designed for `Linux`-based device middleware scenarios, suitable for systems that require stability, extensibility, and observability.

## 🏗️ How to extend business code on top of Sparrow

The recommended pattern is to extend by capability boundary rather than placing business logic directly in `Core/`:

- `Components/`: service components and business logic modules
- `PublicAPIs/`: external interfaces exposed to upper-layer business code
- `PrivateAPIs/`: internal protocols, events, and low-level communication logic
- `Core/`: keep only common framework capabilities to avoid business coupling
- `Hardware/` and `Platform/`: platform differences and hardware adaptation

Typical extension flow:
1. Add a new service or task module under `Components/`
2. Expose the interface in `PublicAPIs/`
3. Use Binder / MQ / Event to communicate with other modules
4. Configure logging and debugging through `Configs/` and `Debug/`

This keeps the core framework stable while isolating business-specific changes in the extension layer, which is easier to maintain and reuse.

## 📚 Documentation

- [Docs Index](Docs/README.md)
- [C++ Programming Guide](Docs/03-Standards/C++编程规范.md)
- [Issue Submission Guidelines](Docs/03-Standards/Issue提交规范.md)
- [Version Management](Docs/04-Development/版本管理.md)
- [Troubleshooting](Docs/06-Troubleshooting/疑难杂症.md)

## 📄 License
This project is licensed under the MIT License. See [LICENSE](LICENSE).

---

## 💎 One-line summary
If you need a solid Linux / embedded C++ foundation for stable runtime, modular extension, and developer-friendly troubleshooting, `Sparrow` is a practical choice.
