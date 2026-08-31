#!/usr/bin/env bash
set -euo pipefail

usage() {
    cat <<'EOF'
Usage:
    gen_coverage_report.sh [--gcov-tool <gcov>] [--output-dir <html_dir>]

Example:
    gen_coverage_report.sh

Note:
    This script only generates coverage report from existing .gcda data.
    It does not execute any test binary.
EOF
}

PROJECT_PATH="$(cd "$(dirname "$0")/../.." && pwd)"
PLATFORM="Default"
GCOV_TOOL=""
HTML_DIR=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        --gcov-tool)
            GCOV_TOOL="$2"
            shift 2
            ;;
        --output-dir)
            HTML_DIR="$2"
            shift 2
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
done

if [[ -z "$GCOV_TOOL" ]]; then
    GCOV_TOOL="gcov"
fi
if ! command -v "$GCOV_TOOL" >/dev/null 2>&1; then
    echo "gcov tool not found: ${GCOV_TOOL}" >&2
    exit 1
fi

RELEASE_DIR="${PROJECT_PATH}/Release/${PLATFORM}"
BUILD_DIR="${RELEASE_DIR}/Cache"
COV_DIR="${RELEASE_DIR}/Coverage"

if [[ -z "$HTML_DIR" ]]; then
    HTML_DIR="${COV_DIR}/html"
fi

if [[ ! -d "$BUILD_DIR" ]]; then
    echo "build cache not found: ${BUILD_DIR}" >&2
    exit 1
fi
mkdir -p "${COV_DIR}"
mkdir -p "${HTML_DIR}"

if ! find "${BUILD_DIR}" -name '*.gcda' -print -quit | grep -q .; then
    echo "No gcda files found under ${BUILD_DIR}." >&2
    echo "Run test binaries first (example): Release/Default/Bin/test_external" >&2
    exit 1
fi

if command -v lcov >/dev/null 2>&1 && command -v genhtml >/dev/null 2>&1; then
    LCOV_CAPTURE_ARGS=(
        --capture
        --directory "${BUILD_DIR}"
        --base-directory "${PROJECT_PATH}"
        --output-file "${COV_DIR}/coverage_full.info"
        --rc lcov_branch_coverage=1
        --gcov-tool "$GCOV_TOOL"
    )

    echo "==> capture coverage with lcov"
    lcov "${LCOV_CAPTURE_ARGS[@]}"

    echo "==> filter third-party and testcase sources"
    lcov \
        --remove "${COV_DIR}/coverage_full.info" \
        '/usr/*' \
        '*/3rdParty/*' \
        '*/TestCase/*' \
        --output-file "${COV_DIR}/coverage.info" \
        --rc lcov_branch_coverage=1

    echo "==> generate html report"
    genhtml \
        "${COV_DIR}/coverage.info" \
        --output-directory "${HTML_DIR}" \
        --branch-coverage \
        --title "Sparrow ${PLATFORM} Coverage"

    echo "Coverage report: ${HTML_DIR}/index.html"
else
    echo "==> lcov/genhtml not found, fallback to gcov text report"
    RAW_TXT="${COV_DIR}/gcov_raw.txt"
    SUMMARY_TXT="${COV_DIR}/coverage_summary.txt"
    echo "==> output path: ${SUMMARY_TXT}"

    find "${BUILD_DIR}" -name '*.gcda' -print0 \
        | xargs -0 "$GCOV_TOOL" -n -s "$PROJECT_PATH" > "$RAW_TXT"

    awk -v platform_name="$PLATFORM" '
        /^File '\''/ {
            file=$0
            sub(/^File '\''/, "", file)
            sub(/'\''$/, "", file)
            current=file
            next
        }
        /^Lines executed:/ {
            if (current == "") {
                next
            }
            if (current ~ /^\/usr\//) {
                current=""
                next
            }
            if (current ~ /(^|\/)3rdParty\// || current ~ /(^|\/)TestCase\//) {
                current=""
                next
            }
            if (current !~ /\.(c|cc|cpp|cxx)$/) {
                current=""
                next
            }

            line=$0
            sub(/^Lines executed:/, "", line)
            split(line, p, "% of ")
            pct=p[1] + 0.0
            total=p[2] + 0
            if (total <= 0) {
                current=""
                next
            }
            key=current
            seen_total[key]=total
            seen_exec[key]=pct*total/100.0
            current=""
        }
        END {
            total_lines=0
            total_exec=0
            file_cnt=0
            for (k in seen_total) {
                total_lines += seen_total[k]
                total_exec += seen_exec[k]
                file_cnt += 1
            }
            if (total_lines == 0) {
                print "No project source coverage found.";
                exit 1
            }
            pct_all=(total_exec*100.0)/total_lines
            printf("Platform: %s\n", platform_name)
            printf("Files: %d\n", file_cnt)
            printf("Executed lines: %.0f / %.0f\n", total_exec, total_lines)
            printf("Line coverage: %.2f%%\n", pct_all)
        }
    ' "$RAW_TXT" > "$SUMMARY_TXT"

    echo "Coverage summary: ${SUMMARY_TXT}"
fi