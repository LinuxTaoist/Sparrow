# Default 平台说明

## 1. 这是做什么的
Default 平台是工程在 Ubuntu/Linux 环境下的基线平台配置。

作用：
- 提供该平台的编译入口脚本。
- 提供该平台的编译选项与模块开关。
- 提供该平台的运行期配置文件模板。

## 2. 目录结构与职责
Platform/Default 下目录职责如下：

- [Build](Build)：编译入口与构建配置。
- [Configs](Configs)：平台运行配置（如平台属性、日志配置）。
- [3rdParty](3rdParty)：平台相关第三方产物或适配预留目录。

## 3. Build 目录文件含义
Build 目录核心文件：

- [build_default.sh](Build/build_default.sh)：默认编译入口，编译 Debug 版本（支持增量与按目标模块编译）。
- [rebuild_default.sh](Build/rebuild_default.sh)：默认全量重编入口，编译 Release 版本（先清理后从干净状态构建）。
- [Options/Default_compile_options.cmake](Build/Options/Default_compile_options.cmake)：编译参数配置（编译器、告警、优化、宏等）。
- [Options/Default_modules_config.cmake](Build/Options/Default_modules_config.cmake)：模块开关配置（按需启停功能模块）。

## 4. 新平台如何适配
各平台目录结构基本一致，建议使用工具一键创建骨架，再补充平台差异配置。

在工程根目录执行 [xtools.sh](../../Tools/xtools.sh)：

./Tools/xtools.sh new-platform <platform_name>

执行后会自动创建：
- Platform/<platform_name>/Build
- Platform/<platform_name>/Build/Options
- Platform/<platform_name>/Configs
- Platform/<platform_name>/3rdParty
- build_<platform_name>.sh / rebuild_<platform_name>.sh
- <platform_name>_compile_options.cmake / <platform_name>_modules_config.cmake
- vendor.prop / sprlog.conf

创建后建议最少完成以下调整：
- 在 compile_options 中补齐工具链与编译参数。
- 在 modules_config 中设置平台模块开关。
- 在 Configs 中确认平台属性与日志配置。
