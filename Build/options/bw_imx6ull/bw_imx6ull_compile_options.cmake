## baiwen imx6ull compile options
message("load bw_imx6ull_compile_options.cmake")

set(TOOLCHAIN_DIR "/opt/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/bin/")
set(TOOLCHAIN_PREFIX "arm-linux-gnueabihf")
set(CMAKE_C_COMPILER "${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}-gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}-g++")

## 可选功能模块配置
# 非核心模块，编译时默认禁用
# 启用方法：取消对应行的注释符号(#)
# set(BUILD_DEBUG ON)       # 调试工具集（含调试符号与增强日志）
# set(BUILD_EXAMPLES ON)    # 示例程序（演示API用法，非生产环境）
# set(BUILD_TESTCASE ON)    # 单元测试套件（依赖gtest库）
