#!/bin/bash

set -e
SCRIPT_REAL_PATH=$(readlink -f "$0")
SCRIPT_REAL_DIR=$(dirname "${SCRIPT_REAL_PATH}")
PROJECT_PATH=$(readlink -f "${SCRIPT_REAL_DIR}/../../..")

BUILD_TYPE="Release"
PROJECT_PLATFORM="Default"
OUTPUT_PATH="${PROJECT_PATH}/Release/${PROJECT_PLATFORM}"

# 清理第三方编译产物，确保每次 rebuild 都是从干净状态重新构建。
rm -rf $PROJECT_PATH/3rdParty/googletest/lib/$PROJECT_PLATFORM
rm -rf $PROJECT_PATH/3rdParty/sqlite/lib/$PROJECT_PLATFORM
rm -f  $PROJECT_PATH/3rdParty/sqlite/include/sqlite3.h
rm -f  $PROJECT_PATH/3rdParty/sqlite/include/sqlite3ext.h
rm -rf $PROJECT_PATH/3rdParty/sqlite/.sqlite_build_$PROJECT_PLATFORM
rm -rf $PROJECT_PATH/Platform/$PROJECT_PLATFORM/3rdParty/googletest/.gtest_build
rm -rf $PROJECT_PATH/Platform/$PROJECT_PLATFORM/3rdParty/sqlite/.sqlite_build
rm -rf $OUTPUT_PATH/*

mkdir -p $OUTPUT_PATH/Cache
mkdir -p $OUTPUT_PATH/Include
mkdir -p $OUTPUT_PATH/Etc
mkdir -p $OUTPUT_PATH/Lib

# 记录开始时间
START_TIME=$(date +%s)
START_TIME_HUMAN=$(date +"%Y-%m-%d %H:%M:%S")

cd $OUTPUT_PATH/Cache/
cmake $PROJECT_PATH \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE          \
    -DPROJECT_PLATFORM=$PROJECT_PLATFORM    \
    -DCMAKE_INSTALL_PREFIX=$OUTPUT_PATH     \
    -DOUTPUT_PATH=$OUTPUT_PATH

make -j16

# 记录结束时间并计算耗时
END_TIME=$(date +%s)
END_TIME_HUMAN=$(date +"%Y-%m-%d %H:%M:%S")
ELAPSED_TIME=$((END_TIME - START_TIME))

# 格式化耗时为时分秒
ELAPSED_H=$((ELAPSED_TIME / 3600))
ELAPSED_M=$(((ELAPSED_TIME % 3600) / 60))
ELAPSED_S=$((ELAPSED_TIME % 60))

# 输出结果
echo "====================================="
echo "编译开始于: $START_TIME_HUMAN"
echo "编译结束于: $END_TIME_HUMAN"
echo "总编译时间: $ELAPSED_H 小时 $ELAPSED_M 分钟 $ELAPSED_S 秒 ($ELAPSED_TIME 秒)"

