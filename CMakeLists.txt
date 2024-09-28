cmake_minimum_required(VERSION 2.8)
project(Sparrow)

# 兼容CMAKE旧版本
cmake_policy(SET CMP0046 OLD)

## 保存工程路径
set(PROJECT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

## 包含配置文件
include(${PROJECT_PATH}/version.cmake)
include(${PROJECT_PATH}/modules_configs.cmake)
include(${PROJECT_PATH}/Build/options/${PROJECT_PLATFORM}/${PROJECT_PLATFORM}_compile_options.cmake)

## 设置全局位置无关代码标志
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

## 保存输出路径
set(OUTPUT_PATH             ${PROJECT_PATH}/Release)
set(LIBRARY_OUTPUT_PATH     ${OUTPUT_PATH}/Lib)
set(EXECUTABLE_OUTPUT_PATH  ${OUTPUT_PATH}/Bin)

## 链接库路径
link_directories(${OUTPUT_PATH}/Lib)

option(TEST_DEBUG "option for debug" OFF)
if (TEST_DEBUG)
    add_definitions(-DTEST_DEBUG)
endif()

## 打开编译警告
set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-unused-parameter -Wno-ignored-qualifiers")

set(CMAKE_CXX_STANDARD  11)
set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS} -Wall -g3")
set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -Wall -g3")

## 工程信息
string(TIMESTAMP CURRENT_TIME "%Y-%m-%d %H:%M:%S")
cmake_host_system_information(RESULT HOSTNAME QUERY HOSTNAME)

## 打印工程信息
message("======================================================================")
message("Project        : ${PROJECT_NAME} ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION}")
message("C++ Standard   : ${CMAKE_CXX_STANDARD}")
message("G++ Version    : ${CMAKE_CXX_COMPILER_VERSION}")
message("Gcc Version    : ${CMAKE_C_COMPILER_VERSION}")
message("Running Env    : ${PROJECT_PLATFORM}")
message("Build Time     : ${CURRENT_TIME}")
message("Build Type     : ${CMAKE_BUILD_TYPE}")
message("Build Host     : ${HOSTNAME}")
message("Build Platform : ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR}")
message("Module Config  : ${MODULE_CONFIG_VERSION}")
message("OUTPUT_PATH    : ${OUTPUT_PATH}")
message("======================================================================")

add_definitions(-DPROJECT_INFO="${PROJECT_NAME} ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION}")
add_definitions(-DCXX_STANDARD="${CMAKE_CXX_STANDARD}")
add_definitions(-DGXX_VERSION="${CMAKE_CXX_COMPILER_VERSION}")
add_definitions(-DGCC_VERSION="${CMAKE_C_COMPILER_VERSION}")
add_definitions(-DRUN_ENV="${PROJECT_PLATFORM}")
add_definitions(-DBUILD_TIME="${CURRENT_TIME}")
add_definitions(-DBUILD_TYPE="${CMAKE_BUILD_TYPE}")
add_definitions(-DBUILD_HOST="${HOSTNAME}")
add_definitions(-DBUILD_PLATFORM="${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
add_definitions(-DMODULE_CONFIG_VERSION="${MODULE_CONFIG_VERSION}")

add_subdirectory(3rdParty)
add_subdirectory(3rdAdapter)
add_subdirectory(Components)
add_subdirectory(Core)
add_subdirectory(Debug)
add_subdirectory(Examples)
add_subdirectory(PrivateAPIs)
add_subdirectory(PublicAPIs)
add_subdirectory(UtilModules)
