#!/bin/bash
set -euo pipefail

usage() {
    cat <<'EOF'
Usage:
    stop_valgrind.sh [--report-path <dir>]

Options:
    --report-path <dir>  start_valgrind.sh 的报告输出目录，默认当前路径下 valgrind_logs
    -h, --help           显示帮助
EOF
}

REPORT_PATH="$(pwd)/valgrind_logs"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --report-path)
            REPORT_PATH="$2"
            shift 2
            ;;
        --report-path=*)
            REPORT_PATH="${1#*=}"
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "未知参数: $1" >&2
            usage
            exit 1
            ;;
    esac
done

PID_FILE="$REPORT_PATH/service_pids.txt"

if [ ! -f "$PID_FILE" ]; then
    echo "未发现运行中的服务（PID 文件不存在：$PID_FILE）" >&2
    exit 1
fi

echo "按启动反序停止所有服务..."

tac "$PID_FILE" | while read -r service pid; do
    if kill -0 "$pid" 2>/dev/null; then
        echo "停止 ${service} (PID: ${pid})"
        kill -10 "$pid" 2>/dev/null || true
        sleep 2

        if kill -0 "$pid" 2>/dev/null; then
            kill -9 "$pid" 2>/dev/null || true
            echo "${service} (PID: ${pid}) 已强制终止"
        fi
    else
        echo "${service} (PID: ${pid}) 已停止"
    fi
done

rm -f "$PID_FILE"
echo "所有服务已停止，报告目录：${REPORT_PATH}"
