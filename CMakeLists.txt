cmake_minimum_required(VERSION 3.20)
project(Sparrow)

# 兼容CMAKE旧版本
cmake_policy(SET CMP0046 OLD)

set(CMAKE_CXX_STANDARD 11)


## 保存工程路径
set(PROJECT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

## 保存输出路径
set(OUTPUT_PATH            ${PROJECT_PATH}/Release)
set(LIBRARY_OUTPUT_PATH    ${OUTPUT_PATH}/Lib)
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_PATH}/Bin)

## 链接库路径
link_directories(${OUTPUT_PATH}/Lib)

## add_definitions(-DFOO -DDEBUG ...)
# option(TEST_DEBUG "option for debug" OFF)
# if (TEST_DEBUG)
#     add_definitions(-DTEST_DEBUG)
# endif()

## 打开编译警告
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-unused-parameter -Wno-ignored-qualifiers")

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -g3 -O0 -fpermissive")

## 保存编译时间
string(TIMESTAMP CURRENT_TIME "%Y-%m-%d %H:%M:%S")
cmake_host_system_information(RESULT HOSTNAME QUERY HOSTNAME)

## 打印工程信息
message("======================================================================")
message("Project      : ${PROJECT_NAME}")
message("Version      : ${PROJECT_VERSION_NUMBER}")
message("Host Name    : ${HOSTNAME}")
message("Build Time   : ${CURRENT_TIME}")
message("Build Type   : ${CMAKE_BUILD_TYPE}")
message("C++ Standard : ${CMAKE_CXX_STANDARD}")
message("g++ version  : ${CMAKE_CXX_COMPILER_VERSION}")
message("gcc version  : ${CMAKE_C_COMPILER_VERSION}")
message("Platform     : ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
message("Architecture : ${CMAKE_SYSTEM_PROCESSOR}")
message("======================================================================")

add_subdirectory(Core)
add_subdirectory(Components)
add_subdirectory(Util)
add_subdirectory(Examples)
add_subdirectory(3rdParty)
