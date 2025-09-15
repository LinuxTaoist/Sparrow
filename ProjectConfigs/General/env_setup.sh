#!/bin/bash

SCRIPT_PATH=$(pwd)
RELEASE_PATH=$(dirname "$SCRIPT_PATH")
BIN_PATH="$RELEASE_PATH/Bin"
LIB_PATH="$RELEASE_PATH/Lib"

# 检查并设置可执行权限
if [ -w "$BIN_PATH" ]; then
    chmod +x "$BIN_PATH"/*
else
    echo "Error: $BIN_PATH is not writable."
fi

# 检查 LIB_PATH 是否已经在 LD_LIBRARY_PATH 中
if [[ ":$LD_LIBRARY_PATH:" != *":$LIB_PATH:"* ]]; then
    export LD_LIBRARY_PATH="$LIB_PATH:$LD_LIBRARY_PATH"
fi

# 设置时区 东八区
export TZ=CST-8
echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
