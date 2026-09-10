# modules_config.cmake
message("load Default_modules_config.cmake")

## 模块加载配置版本号
set(MODULE_CONFIG_VERSION "DEFAULT_MCONFIG_1002")

# Hardware 平台
set(HAL_FAMILY "Default")

## 基础模块配置
# 增加变量名对应Components/Basics路径下的文件夹名
# 启用方法：
#   - 修改路径： 当前文件
#   - 修改方式： 增加/删除对应行的注释符号(#)
list(APPEND BASIC_MODULES BinderManagerSrv)
list(APPEND BASIC_MODULES ConfigManagerSrv)
list(APPEND BASIC_MODULES DebugSrv)
list(APPEND BASIC_MODULES LogManagerSrv)
list(APPEND BASIC_MODULES MediatorSrv)
list(APPEND BASIC_MODULES PowerManagerSrv)
list(APPEND BASIC_MODULES PropertiesSrv)
list(APPEND BASIC_MODULES ServiceManagerSrv)
list(APPEND BASIC_MODULES SparrowSrv)
list(APPEND BASIC_MODULES StatusMonitorSrv)

## 可选功能模块配置
# 非核心模块，编译时默认禁用
# 启用方法：增加/删除对应行的注释符号(#)
set(BUILD_DEBUG ON)       # 调试工具集（含调试符号与增强日志）
set(BUILD_EXAMPLES ON)    # 示例程序（演示API用法，非生产环境）
set(BUILD_TESTCASE ON)    # 单元测试套件（依赖gtest库）
set(BUILD_COVERAGE ON)    # 覆盖率开关（仅在需要生成覆盖率报告时开启）
set(BUILD_ASAN OFF)       # ASan内存检测 （依赖libasan.so）
