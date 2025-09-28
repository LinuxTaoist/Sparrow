## default compile options
message("load default_compile_options.cmake")

## 模块配置版本控制
# 用于编译时标识配置文件版本，影响：
# - 编译命令行输出版本信息
# - 运行时 /tmp/sparrow_version 文件内容
set(MODULE_CONFIG_VERSION "DEFAULT_MCONFIG_1002")

## 设置链接选项
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")

## 可选功能模块配置
# 非核心模块，编译时默认禁用
# 启用方法：取消对应行的注释符号(#)
set(BUILD_DEBUG ON)       # 调试工具集（含调试符号与增强日志）
# set(BUILD_EXAMPLES ON)    # 示例程序（演示API用法，非生产环境）
# set(BUILD_TESTCASE ON)    # 单元测试套件（依赖gtest库）
