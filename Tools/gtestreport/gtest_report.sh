#!/usr/bin/env bash
set -euo pipefail

usage() {
    cat <<'EOF'
Usage:
    gtest_report.sh --run[=<bin_path>] [--tool=<builtin|junit2html>] [--output=<dir>]
    gtest_report.sh --xml=<xml_file> [--tool=<builtin|junit2html>] [--output=<dir>]

Examples:
    gtest_report.sh --run                              # 跑默认用例并生成 html（Default/Ubuntu）
    gtest_report.sh --run=test_internal                # 跑指定 bin 名
    gtest_report.sh --run=./my_test --tool=junit2html  # 跑指定路径 bin，用 junit2html 转换
    gtest_report.sh --xml=test_internal.xml            # 将 xml 转成 html
    gtest_report.sh --xml=a.xml --xml=b.xml            # 多个 xml 合并转换

Options:
    --run[=<bin_path>]  运行测试并生成报告，不指定则跑默认用例
    --xml=<xml_file>    将指定 xml 转成 html，可多次指定
    --tool=<tool>       转换工具 builtin|junit2html，默认 builtin
    --output=<dir>      输出目录，默认当前目录

Note:
    --run 仅适用于 Default(Ubuntu) 平台，会先等待服务就绪再运行测试（不自动拉起服务）。
    其他平台请先在目标板运行测试生成 xml，再用 --xml 转换。
EOF
}

PROJECT_PATH="$(cd "$(dirname "$0")/../.." && pwd)"
PLATFORM="Default"
MODE=""          # run | xml
RUN_BIN=""       # --run=<path>，空表示默认用例
XML_FILES=()
TOOL="builtin"
OUTPUT_DIR=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        --run)
            MODE="run"
            ;;
        --run=*)
            MODE="run"
            RUN_BIN="${1#*=}"
            ;;
        --xml=*)
            MODE="xml"
            XML_FILES+=("${1#*=}")
            ;;
        --tool=*)
            TOOL="${1#*=}"
            ;;
        --output=*)
            OUTPUT_DIR="${1#*=}"
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "unknown arg: $1" >&2
            usage
            exit 1
            ;;
    esac
    shift
done

if [[ -z "$MODE" ]]; then
    echo "error: need --run or --xml" >&2
    usage
    exit 1
fi

if [[ "$TOOL" != "builtin" && "$TOOL" != "junit2html" ]]; then
    echo "invalid tool: $TOOL (support: builtin, junit2html)" >&2
    exit 1
fi

JUNIT2HTML_BIN="$(command -v junit2html 2>/dev/null || true)"

RELEASE_DIR="${PROJECT_PATH}/Release/${PLATFORM}"
BIN_DIR="${RELEASE_DIR}/Bin"
REPORT_DIR="${RELEASE_DIR}/TestReport"

if [[ -z "$OUTPUT_DIR" ]]; then
    OUTPUT_DIR="$(pwd)"
fi

run_tests() {
    echo "==> wait for services (binder/config) ready"
    local ready=0
    for _ in $(seq 1 30); do
        if [[ -e /tmp/bindermanagersrv && -e /tmp/configmanagersrv ]]; then
            ready=1
            break
        fi
        sleep 0.3
    done

    if [[ "$ready" -ne 1 ]]; then
        echo "error: services not ready, please start servicemanagersrv first" >&2
        return 1
    fi
    echo "==> services ready"

    mkdir -p "$REPORT_DIR"
    local failed=0

    local bins=()
    if [[ -n "$RUN_BIN" ]]; then
        # 含 / 视为路径（相对 cwd），否则从 BIN_DIR 找
        if [[ "$RUN_BIN" == */* ]]; then
            bins+=("$RUN_BIN")
        else
            bins+=("$BIN_DIR/$RUN_BIN")
        fi
    else
        bins+=("$BIN_DIR/test_internal" "$BIN_DIR/test_external")
    fi

    for bin in "${bins[@]}"; do
        local name
        name="$(basename "$bin")"
        echo "==> run ${name}"
        "$bin" --gtest_output="xml:${REPORT_DIR}/${name}.xml" || failed=1
    done

    if [[ "$failed" -ne 0 ]]; then
        echo "==> warning: some tests failed, report will still be generated." >&2
    fi
}

if [[ "$MODE" == "run" ]]; then
    run_tests
    # 跑完用例后，从 REPORT_DIR 收集 xml
    if [[ ${#XML_FILES[@]} -eq 0 ]]; then
        while IFS= read -r f; do
            XML_FILES+=("$f")
        done < <(find "$REPORT_DIR" -maxdepth 1 -name '*.xml' -type f | sort)
    fi
fi

if [[ ${#XML_FILES[@]} -eq 0 ]]; then
    echo "error: no gtest xml files found" >&2
    exit 1
fi

mkdir -p "$OUTPUT_DIR"

if [[ "$TOOL" == "junit2html" ]]; then
    if [[ -z "$JUNIT2HTML_BIN" ]]; then
        echo "error: junit2html not found, install it first: pip3 install junit2html" >&2
        exit 1
    fi

    echo "==> convert with junit2html"

    if [[ ${#XML_FILES[@]} -eq 1 ]]; then
        "$JUNIT2HTML_BIN" "${XML_FILES[0]}" "$OUTPUT_DIR/index.html"
    else
        MERGED_XML="$OUTPUT_DIR/merged.xml"
        "$JUNIT2HTML_BIN" --merge "$MERGED_XML" "${XML_FILES[@]}"
        "$JUNIT2HTML_BIN" "$MERGED_XML" "$OUTPUT_DIR/index.html"
        rm -f "$MERGED_XML"
    fi

    echo "GTest report: $OUTPUT_DIR/index.html"
    exit 0
fi

python3 - "$OUTPUT_DIR" "${XML_FILES[@]}" <<'PYEOF'
import sys
import os
import html as html_mod
import xml.etree.ElementTree as ET

output_dir = sys.argv[1]
xml_files = sys.argv[2:]

def parse_xml(path):
    tree = ET.parse(path)
    root = tree.getroot()
    suites = []
    for suite in root.findall('testsuite'):
        name = suite.get('name', '')
        tests = int(suite.get('tests', 0))
        failures = int(suite.get('failures', 0))
        errors = int(suite.get('errors', 0))
        disabled = int(suite.get('disabled', 0))
        time = float(suite.get('time', 0))
        cases = []
        for case in suite.findall('testcase'):
            cname = case.get('name', '')
            ctime = float(case.get('time', 0))
            failure = case.find('failure')
            error = case.find('error')
            if failure is not None:
                status, msg = 'failed', failure.get('message', '')
            elif error is not None:
                status, msg = 'error', error.get('message', '')
            else:
                status, msg = 'passed', ''
            cases.append((cname, status, ctime, msg))
        suites.append((name, tests, failures, errors, disabled, time, cases))
    return suites

all_suites = []
for f in xml_files:
    all_suites.extend(parse_xml(f))

total_tests = sum(s[1] for s in all_suites)
total_failures = sum(s[2] for s in all_suites)
total_errors = sum(s[3] for s in all_suites)
total_disabled = sum(s[4] for s in all_suites)
total_time = sum(s[5] for s in all_suites)
passed = total_tests - total_failures - total_errors
pass_rate = (passed * 100.0 / total_tests) if total_tests else 0.0

suite_rows = []
for name, tests, failures, errors, disabled, time, cases in all_suites:
    sp = tests - failures - errors
    state = 'ok' if (failures == 0 and errors == 0) else 'bad'
    suite_rows.append(
        f'<tr><td>{html_mod.escape(name)}</td>'
        f'<td>{tests}</td>'
        f'<td class="ok">{sp}</td>'
        f'<td class="{state}">{failures}</td>'
        f'<td class="{state}">{errors}</td>'
        f'<td>{disabled}</td>'
        f'<td>{time:.3f}s</td></tr>'
    )

fail_rows = []
for name, tests, failures, errors, disabled, time, cases in all_suites:
    for cname, status, ctime, msg in cases:
        if status != 'passed':
            fail_rows.append(
                f'<tr><td>{html_mod.escape(name)}</td>'
                f'<td>{html_mod.escape(cname)}</td>'
                f'<td class="bad">{status}</td>'
                f'<td class="msg">{html_mod.escape(msg[:300])}</td></tr>'
            )

suite_rows_html = '\n'.join(suite_rows)
fail_rows_html = '\n'.join(fail_rows) if fail_rows else '<tr><td colspan="4" class="ok">All tests passed</td></tr>'

html_doc = f'''<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="utf-8">
<title>GTest Report</title>
<style>
body {{ font-family: -apple-system, "Segoe UI", "Microsoft YaHei", sans-serif; margin: 32px; color: #24292f; }}
h1 {{ font-size: 22px; }}
.summary {{ display: flex; gap: 16px; flex-wrap: wrap; margin: 20px 0; }}
.card {{ background: #f6f8fa; border: 1px solid #d0d7de; border-radius: 8px; padding: 12px 20px; min-width: 110px; }}
.card .num {{ font-size: 26px; font-weight: 600; }}
.card .lbl {{ font-size: 12px; color: #57606a; }}
.ok {{ color: #1a7f37; }}
.bad {{ color: #cf222e; }}
table {{ border-collapse: collapse; width: 100%; margin: 16px 0; }}
th, td {{ border: 1px solid #d0d7de; padding: 6px 10px; text-align: left; font-size: 13px; }}
th {{ background: #f6f8fa; }}
.msg {{ font-size: 12px; color: #57606a; max-width: 500px; word-break: break-all; }}
</style>
</head>
<body>
<h1>GTest 测试结果报告</h1>
<div class="summary">
  <div class="card"><div class="num">{total_tests}</div><div class="lbl">总用例</div></div>
  <div class="card"><div class="num ok">{passed}</div><div class="lbl">通过</div></div>
  <div class="card"><div class="num bad">{total_failures}</div><div class="lbl">失败</div></div>
  <div class="card"><div class="num bad">{total_errors}</div><div class="lbl">错误</div></div>
  <div class="card"><div class="num">{total_disabled}</div><div class="lbl">跳过</div></div>
  <div class="card"><div class="num">{pass_rate:.1f}%</div><div class="lbl">通过率</div></div>
  <div class="card"><div class="num">{total_time:.1f}s</div><div class="lbl">总耗时</div></div>
</div>
<h2>测试套件</h2>
<table>
<tr><th>套件</th><th>用例数</th><th>通过</th><th>失败</th><th>错误</th><th>跳过</th><th>耗时</th></tr>
{suite_rows_html}
</table>
<h2>失败/错误用例</h2>
<table>
<tr><th>套件</th><th>用例</th><th>状态</th><th>信息</th></tr>
{fail_rows_html}
</table>
</body>
</html>'''

out = os.path.join(output_dir, 'index.html')
with open(out, 'w', encoding='utf-8') as fp:
    fp.write(html_doc)

print(f'GTest report: {out}')
print(f'Total: {total_tests} tests, {passed} passed, {total_failures} failed, {total_errors} errors ({pass_rate:.1f}%)')
PYEOF
