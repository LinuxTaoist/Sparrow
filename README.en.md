## Introduction
`Sparrow` is a Linux middleware framework based on the event-driven model. It adopts a microservices architecture and aims to quickly build an efficient Linux C++ middleware application framework.

## Application Scenarios
- Smart Hardware: Such as smart bulbs, smart sockets, smart cameras, robot control systems, etc., suitable for devices that require efficient communication and control.
- Consumer Electronics: For example, smartwatches, health monitoring bracelets, smart TVs, etc., supporting data collection, analysis, and user interface management.
- Vehicle-mounted Systems: Including in-vehicle T-Box, smart cockpits, etc., providing functions such as navigation, entertainment, and phone connection to enhance the driving experience.
- Internet of Things (IoT): Covering smart homes, environmental monitoring, IoT gateways, etc., enabling device interconnection, data collection, and processing.

## Software Architecture
It adopts a microservices architecture, decomposing the system functions into multiple independent modules, with each module responsible for a specific function. Modules communicate with each other through a message publish-subscribe mechanism to ensure high-level decoupling and flexible expansion.

The preliminary framework concept is as follows:

![架构图](Docs/png/架构图.png)

<span style="font-size: 12px;">
<span style="color: blue;">Note: The architecture diagram will be updated at any time with version iterations.</span>
</span>

## Source Code Directory

```c++
Sparrow/
├── 3rdAdapter                // Third-party software adaptation interface
├── 3rdParty                  // Third-party software libraries
├── Build                     // Build scripts
├── CMakeLists.txt
├── Components                // Independent components
├── Core                      // Core framework
├── Debug                     // Debugging tools
├── Docs                      // Documentation
├── Examples                  // Examples
├── Hardware                  // Hardware adaptation interface
├── LICENSE                   // License
├── PrivateAPIs               // Internally public interfaces
├── ProjectConfigs            // Project configurations
├── PublicAPIs                // Externally public interfaces
├── README.en.md
├── README.md
├── Release                   // Release path
├── StaticScans               // Static scan repair records
├── Test                      // Test cases
├── Tools                     // Commonly used auxiliary scripts/executable programs
├── Util                      // General utility functions
├── UtilModules               // General module libraries
├── modules_configs.cmake     // Module configurations
└── version.cmake             // Version information
```

## Usage Tutorial

### Environment
The current local development environment of the project is as follows. Try to keep it consistent with this environment; otherwise, compilation errors may occur.
```Shell
C++ Standard   : 11
G++ Version    : 11.4.0
Gcc Version    : 11.4.0
Build Platform : Ubuntu 18.04.6 LTS
```

### Compilation
① Compile sparrow
```Shell
$ cd Build
$ ./final_general_build.sh
```

② Version generation path
```shell
$ cd Release/
$ ls
Bin  Cache  Include  Lib
```

③ Program execution
After the project is compiled, an executable program will be generated in the `Release/Bin` directory. By executing the `servicemanager` program, all services will be automatically launched, and service monitoring will be started. Refer to `Release/Bin/init.conf` for the service launch configuration.

### Start Services
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

### View Logs in Real Time
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

### Internal Command Set
The project integrates the following command set. You can view the specific command set supported by a process by running `echo help > /tmp/{process name}`, and the response will be in the log:

* Query the commands supported by `statusmonitorsrv`
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

* Query the commands supported by `servicemanagersrv`

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

### Debugging Programs
The project also integrates debugging programs as follows:
* infrawatch
View the running status of each component in real time. First, start the `servicemanagersrv` to launch the services of each component.
```
$ ./infrawatch
==================================  MAIN MENU  ==================================

    1. Display All Message Queues
    2. Manager's Entrance
    3. Custom Debug Options

    [Q] Quit

=================================================================================
```

* property_set, property_get
Used to set and get system properties during terminal debugging
```shell
$ ./property_get ro.SoftwareVersion
1.0.0
```

## Third-Party Library Dependencies
| Third-Party Library | Dependent Module | Remarks |
| ------------------- | ---------------- | ------ |
| sqlite              | ConfigManager    | Used for storing key configuration and other data during project operation |
| googletest          | Test             | Used for internal white-box testing and external black-box testing of the project |

## Documentation
During the project process, some documents will be generated to record some details of the development, doubts, and key solutions worthy of recording, etc.
```
Docs/
├── C++ Programming Specification.md
├── Issue Submission Specification.md
├── Sparrow Q&A.md
├── TODO.md
├── png
├── share
├── spec
├── tips
├── Notes.md
├── Version Management.md
├── Troubleshooting.md
└── Third-Party Dependencies.md
```

**Documents to Pay Attention To**

- [C++ Programming Specification.md](Docs/C++ Programming Specification.md)
The reference basis for the project code specification. It is not mandatory, and it is preferred to be consistent with the existing style.
- [Issue Submission Specification.md](Docs/Issue Submission Specification.md)
The reference template for submitting Github/Gitee Issues.

## Finally
Once in this line of work, strive for progress and excellence; every small step counts towards a thousand miles, and every small stream contributes to a vast ocean. Keep moving forward, and you will eventually reach excellence!