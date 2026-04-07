#!/bin/bash
# 记录开始时间
START_TIME=$(date +%s)
START_TIME_HUMAN=$(date +"%Y-%m-%d %H:%M:%S")

SCRIPT_REAL_PATH=$(readlink -f "$0")
SCRIPT_REAL_DIR=$(dirname "${SCRIPT_REAL_PATH}")
PROJECT_PATH=$(readlink -f "${SCRIPT_REAL_DIR}/../../..")

BUILD_TYPE="Release"
PROJECT_PLATFORM="Default"

mkdir -p $PROJECT_PATH/Release/Cache
mkdir -p $PROJECT_PATH/Release/Include
mkdir -p $PROJECT_PATH/Release/Lib

cd $PROJECT_PATH/Release/Cache/
cmake $PROJECT_PATH \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE          \
    -DPROJECT_PLATFORM=$PROJECT_PLATFORM    \
    -DCMAKE_INSTALL_PREFIX=$PROJECT_PATH/Release/

make -j8

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
