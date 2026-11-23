## default compile options
message("load Default_compile_options.cmake")

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

if(BUILD_CURL)
    set(CURL_CROSS_FLAGS "-fPIC")
    set(CURL_DST_LIB "${PROJECT_PATH}/3rdParty/curl/lib/${PROJECT_PLATFORM}")

    if(NOT EXISTS "${CURL_DST_LIB}/libcurl.a")
        message(STATUS "curl 库缺失，开始自动构建: ${PROJECT_PLATFORM}")
        set(CURL_BUILD_ARGS
            bash
            ${PROJECT_PATH}/3rdParty/curl/build.sh
            --project-path ${PROJECT_PATH}
            --platform ${PROJECT_PLATFORM}
            --c-compiler ${CMAKE_C_COMPILER}
            --cxx-compiler ${CMAKE_CXX_COMPILER}
            --c-flags "${CURL_CROSS_FLAGS}"
            --cxx-flags "${CURL_CROSS_FLAGS}"
        )
        if(CMAKE_SYSROOT)
            list(APPEND CURL_BUILD_ARGS --sysroot ${CMAKE_SYSROOT})
        endif()
        execute_process(COMMAND ${CURL_BUILD_ARGS} RESULT_VARIABLE curl_build_result)
        if(NOT curl_build_result EQUAL 0)
            message(FATAL_ERROR "curl 自动构建失败，平台: ${PROJECT_PLATFORM}")
        endif()
    endif()

    message(STATUS "BUILD_CURL 已启用，curl 库路径: ${CURL_DST_LIB}")
endif()

if(BUILD_GDB)
    set(GDB_CROSS_FLAGS "-fPIC")
    set(GDB_DST_BIN "${PROJECT_PATH}/3rdParty/gdb/bin/${PROJECT_PLATFORM}")

    if(NOT EXISTS "${GDB_DST_BIN}/gdb")
        message(STATUS "gdb 二进制缺失，开始自动构建: ${PROJECT_PLATFORM}")
        set(GDB_BUILD_ARGS
            bash
            ${PROJECT_PATH}/3rdParty/gdb/build.sh
            --project-path ${PROJECT_PATH}
            --platform ${PROJECT_PLATFORM}
            --c-compiler ${CMAKE_C_COMPILER}
            --cxx-compiler ${CMAKE_CXX_COMPILER}
            --c-flags "${GDB_CROSS_FLAGS}"
            --cxx-flags "${GDB_CROSS_FLAGS}"
        )
        if(CMAKE_SYSROOT)
            list(APPEND GDB_BUILD_ARGS --sysroot ${CMAKE_SYSROOT})
        endif()
        execute_process(COMMAND ${GDB_BUILD_ARGS} RESULT_VARIABLE gdb_build_result)
        if(NOT gdb_build_result EQUAL 0)
            message(FATAL_ERROR "gdb 自动构建失败，平台: ${PROJECT_PLATFORM}")
        endif()
    endif()

    message(STATUS "BUILD_GDB 已启用，gdb 路径: ${GDB_DST_BIN}/gdb")
endif()

if(BUILD_I2C_TOOLS)
    set(I2C_TOOLS_CROSS_FLAGS "-fPIC")
    set(I2C_TOOLS_DST_BIN "${PROJECT_PATH}/3rdParty/i2c-tools/bin/${PROJECT_PLATFORM}")

    if(NOT EXISTS "${I2C_TOOLS_DST_BIN}/i2cdetect"
       OR NOT EXISTS "${I2C_TOOLS_DST_BIN}/i2cdump"
       OR NOT EXISTS "${I2C_TOOLS_DST_BIN}/i2cget"
       OR NOT EXISTS "${I2C_TOOLS_DST_BIN}/i2cset"
       OR NOT EXISTS "${I2C_TOOLS_DST_BIN}/i2ctransfer")
        message(STATUS "i2c-tools 二进制缺失，开始自动构建: ${PROJECT_PLATFORM}")
        set(I2C_TOOLS_BUILD_ARGS
            bash
            ${PROJECT_PATH}/3rdParty/i2c-tools/build.sh
            --project-path ${PROJECT_PATH}
            --platform ${PROJECT_PLATFORM}
            --c-compiler ${CMAKE_C_COMPILER}
            --cxx-compiler ${CMAKE_CXX_COMPILER}
            --c-flags "${I2C_TOOLS_CROSS_FLAGS}"
            --cxx-flags "${I2C_TOOLS_CROSS_FLAGS}"
        )
        if(CMAKE_SYSROOT)
            list(APPEND I2C_TOOLS_BUILD_ARGS --sysroot ${CMAKE_SYSROOT})
        endif()
        execute_process(COMMAND ${I2C_TOOLS_BUILD_ARGS} RESULT_VARIABLE i2c_tools_build_result)
        if(NOT i2c_tools_build_result EQUAL 0)
            message(FATAL_ERROR "i2c-tools 自动构建失败，平台: ${PROJECT_PLATFORM}")
        endif()
    endif()

    message(STATUS "BUILD_I2C_TOOLS 已启用，i2c-tools 路径: ${I2C_TOOLS_DST_BIN}")
endif()

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

if(BUILD_ASAN)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "AddressSanitizer enabled (Default platform)")
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
        add_definitions(-DBUILD_ASAN=1)
    else()
        message(FATAL_ERROR "BUILD_ASAN requires GCC or Clang")
    endif()
endif()

if(BUILD_STRIP)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Strip release binaries enabled (Default platform)")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s")
    else()
        message(FATAL_ERROR "BUILD_STRIP requires GCC or Clang")
    endif()
endif()
