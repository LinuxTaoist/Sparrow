#!/bin/bash

BIN_PATH="../../Release/Bin/"

cd $BIN_PATH
LOG_DIR="./valgrind_logs"
PID_FILE="${LOG_DIR}/service_pids.txt"

if [ -f "$PID_FILE" ]; then
  echo "正在按启动反顺序停止所有服务..."

  # 使用tac反向读取PID文件（最后启动的服务先停止）
  tac "$PID_FILE" | while read -r line; do
    pid=$(echo "$line" | awk '{print $NF}')
    service=$(echo "$line" | awk '{print $1}')

    if ps -p "$pid" > /dev/null; then
      echo "停止 ${service} (PID: ${pid})"
      kill -10 "$pid"  # 先尝试正常终止
      sleep 2      # 等待进程优雅退出

      # 若仍未退出，强制终止
      if ps -p "$pid" > /dev/null; then
        kill -9 "$pid"
        echo "${service} (PID: ${pid}) 强制终止"
      fi
    else
      echo "${service} (PID: ${pid}) 已停止"
    fi
  done

  rm -f "$PID_FILE"
  echo "所有服务已按反顺序停止"
else
  echo "未发现运行中的服务（PID文件不存在）"
fi
