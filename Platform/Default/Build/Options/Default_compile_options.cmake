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
set(BUILD_EXAMPLES ON)    # 示例程序（演示API用法，非生产环境）
set(BUILD_TESTCASE ON)    # 单元测试套件（依赖gtest库）

# 拷贝第三方库gtest
if(BUILD_TESTCASE)
    message(STATUS "BUILD_TESTCASE 已启用，开始拷贝 googletest 库文件...")
    set(GTEST_SRC_LIB "${PROJECT_PATH}/Platform/${PROJECT_PLATFORM}/3rdParty/googletest/lib")
    set(GTEST_DST_LIB "${PROJECT_PATH}/3rdParty/googletest/lib/${PROJECT_PLATFORM}")

    file(MAKE_DIRECTORY ${GTEST_DST_LIB})
    file(COPY ${GTEST_SRC_LIB}/libgtest_main.a DESTINATION ${GTEST_DST_LIB}/)
    file(COPY ${GTEST_SRC_LIB}/libgtest.a DESTINATION ${GTEST_DST_LIB}/)
    message(STATUS "googletest 库拷贝完成：${GTEST_SRC_LIB} -> ${GTEST_DST_LIB}")
endif()
