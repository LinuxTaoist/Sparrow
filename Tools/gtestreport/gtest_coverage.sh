#!/usr/bin/env bash
set -euo pipefail

usage() {
    cat <<'EOF'
Usage:
    gtest_coverage.sh [--gcov-tool <gcov>] [--output-dir <html_dir>] [--run-tests] [--strict-html]

Example:
    gtest_coverage.sh
    gtest_coverage.sh --run-tests
    gtest_coverage.sh --run-tests --strict-html

Note:
    Default platform is fixed to "Default".
    With --run-tests, the script runs ctest before collecting coverage.
EOF
}

PROJECT_PATH="$(cd "$(dirname "$0")/../.." && pwd)"
PLATFORM="Default"
GCOV_TOOL=""
HTML_DIR=""
RUN_TESTS=0
STRICT_HTML=0

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
        --run-tests)
            RUN_TESTS=1
            shift
            ;;
        --strict-html)
            STRICT_HTML=1
            shift
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

CMAKE_CACHE_FILE="${BUILD_DIR}/CMakeCache.txt"
if [[ -f "$CMAKE_CACHE_FILE" ]]; then
    if ! grep -q '^BUILD_COVERAGE:BOOL=ON$' "$CMAKE_CACHE_FILE"; then
        # 兼容“平台配置用普通 set 覆盖默认 cache 值”的场景：
        # CMakeCache 可能仍为 OFF，但实际编译参数已包含覆盖率选项。
        HAS_COVERAGE_FLAGS=0
        while IFS= read -r -d '' flag_file; do
            if grep -E -- '--coverage|-fprofile-arcs|-ftest-coverage' "$flag_file" >/dev/null 2>&1; then
                HAS_COVERAGE_FLAGS=1
                break
            fi
        done < <(find "$BUILD_DIR" -type f \( -name 'flags.make' -o -name 'link.txt' \) -print0 2>/dev/null)

        if [[ "$HAS_COVERAGE_FLAGS" -ne 1 ]]; then
            echo "BUILD_COVERAGE is OFF in ${CMAKE_CACHE_FILE}" >&2
            echo "Please enable BUILD_COVERAGE in Platform/Default/Build/Options/Default_modules_config.cmake and rebuild." >&2
            exit 1
        else
            echo "Note: CMakeCache reports BUILD_COVERAGE=OFF, but coverage compile/link flags were detected; continue." >&2
        fi
    fi
fi

run_tests_with_fallback() {
    local ctest_output=""
    local ctest_rc=0

    echo "==> run tests (ctest) to generate gcda"
    (
        cd "$BUILD_DIR"
        ctest_output="$(ctest --output-on-failure 2>&1)" || ctest_rc=$?
        echo "$ctest_output"

        if grep -q 'No tests were found!!!' <<<"$ctest_output"; then
            local test_bin_dir="${RELEASE_DIR}/Bin"
            local test_bins=()
            local test_bin=""
            local failed=0
            local run_benchmark="${RUN_BENCHMARK_TESTS:-0}"

            echo "==> ctest has no registered tests, fallback to run test_* binaries"
            if [[ -d "$test_bin_dir" ]]; then
                while IFS= read -r test_bin; do
                    local bin_name
                    bin_name="$(basename "$test_bin")"

                    # 覆盖率默认只跑功能测试，避免 benchmark 压测日志淹没输出并拖慢流程。
                    if [[ "$bin_name" == "test_benchmark" && "$run_benchmark" != "1" ]]; then
                        echo "==> skip ${test_bin} (set RUN_BENCHMARK_TESTS=1 to include)"
                        continue
                    fi

                    # 优先保留 internal/external；其他 test_* 保持兼容继续执行。
                    test_bins+=("$test_bin")
                done < <(find "$test_bin_dir" -maxdepth 1 -type f -name 'test_*' -perm -u+x | sort)
            fi

            if [[ ${#test_bins[@]} -eq 0 ]]; then
                echo "No executable test binaries found in ${test_bin_dir}" >&2
                return 1
            fi

            for test_bin in "${test_bins[@]}"; do
                echo "==> run ${test_bin}"
                if ! "$test_bin"; then
                    failed=1
                fi
            done

            if [[ "$failed" -ne 0 ]]; then
                echo "Some fallback test binaries failed." >&2
                return 1
            fi
        elif [[ "$ctest_rc" -ne 0 ]]; then
            return "$ctest_rc"
        fi
    )
}

if [[ "$RUN_TESTS" -eq 1 ]]; then
    run_tests_with_fallback || {
        echo "==> warning: some tests failed, but coverage report will still be generated." >&2
    }
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

    echo "==> coverage summary"
    lcov --summary "${COV_DIR}/coverage.info" --rc lcov_branch_coverage=1 > "${COV_DIR}/coverage_summary.txt"

    echo "==> generate html report"
    genhtml \
        "${COV_DIR}/coverage.info" \
        --output-directory "${HTML_DIR}" \
        --branch-coverage \
        --title "Sparrow ${PLATFORM} Coverage"

    echo "Coverage report: ${HTML_DIR}/index.html"
    echo "Coverage summary: ${COV_DIR}/coverage_summary.txt"
else
    if [[ "$STRICT_HTML" -eq 1 ]]; then
        echo "lcov/genhtml not found, cannot generate html report in strict mode." >&2
        echo "Install them first (Ubuntu): sudo apt install lcov" >&2
        exit 1
    fi

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