## default compile options
message("load default_compile_options.cmake")

## 模块配置版本控制
# 用于编译时标识配置文件版本，影响：
# - 编译命令行输出版本信息
# - 运行时 /tmp/sparrow_version 文件内容
set(MODULE_CONFIG_VERSION "DEFAULT_MCONFIG_1002")

## 设置链接选项
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")

# 构建第三方库gtest
if(BUILD_TESTCASE)
    set(GTEST_DST_LIB "${PROJECT_PATH}/3rdParty/googletest/lib/${PROJECT_PLATFORM}")

    if(NOT EXISTS "${GTEST_DST_LIB}/libgtest.a" OR NOT EXISTS "${GTEST_DST_LIB}/libgtest_main.a")
        message(STATUS "googletest 库缺失，开始自动构建: ${PROJECT_PLATFORM}")
        execute_process(
            COMMAND bash ${PROJECT_PATH}/3rdParty/googletest/build.sh
                    --project-path ${PROJECT_PATH}
                    --platform ${PROJECT_PLATFORM}
                    --c-compiler ${CMAKE_C_COMPILER}
                    --cxx-compiler ${CMAKE_CXX_COMPILER}
            RESULT_VARIABLE gtest_build_result
        )
        if(NOT gtest_build_result EQUAL 0)
            message(FATAL_ERROR "googletest 自动构建失败，平台: ${PROJECT_PLATFORM}")
        endif()
    endif()

    message(STATUS "BUILD_TESTCASE 已启用，googletest 库路径: ${GTEST_DST_LIB}")
endif()

# 构建第三方库sqlite
set(SQLITE_DST_LIB "${PROJECT_PATH}/3rdParty/sqlite/lib/${PROJECT_PLATFORM}")
if(NOT EXISTS "${SQLITE_DST_LIB}/libsqlite3.a")
    message(STATUS "sqlite 库缺失，开始自动构建: ${PROJECT_PLATFORM}")
    execute_process(
        COMMAND bash ${PROJECT_PATH}/3rdParty/sqlite/build.sh
                --project-path ${PROJECT_PATH}
                --platform ${PROJECT_PLATFORM}
                --c-compiler ${CMAKE_C_COMPILER}
        RESULT_VARIABLE sqlite_build_result
    )
    if(NOT sqlite_build_result EQUAL 0)
        message(FATAL_ERROR "sqlite 自动构建失败，平台: ${PROJECT_PLATFORM}")
    endif()
endif()

message(STATUS "sqlite 库路径: ${SQLITE_DST_LIB}")

if(BUILD_COVERAGE)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Coverage instrumentation enabled (Default platform)")
        file(GLOB_RECURSE STALE_GCDA_FILES "${OUTPUT_PATH}/Cache/*.gcda")
        if(STALE_GCDA_FILES)
            message(STATUS "Clean stale coverage data")
            file(REMOVE ${STALE_GCDA_FILES})
        endif()
        set(COVERAGE_FLAGS "-O0 -g --coverage -fprofile-arcs -ftest-coverage")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COVERAGE_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
        add_definitions(-DBUILD_COVERAGE=1)
    else()
        message(FATAL_ERROR "BUILD_COVERAGE requires GCC or Clang")
    endif()
endif()
