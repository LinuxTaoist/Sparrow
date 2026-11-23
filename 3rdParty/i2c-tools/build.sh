#!/usr/bin/env bash

set -euo pipefail

PROJECT_PATH=""
PLATFORM="Default"
C_COMPILER=""
CXX_COMPILER=""
SYSROOT=""
C_FLAGS=""
CXX_FLAGS=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        --project-path)
            PROJECT_PATH="$2"
            shift 2
            ;;
        --platform)
            PLATFORM="$2"
            shift 2
            ;;
        --c-compiler)
            C_COMPILER="$2"
            shift 2
            ;;
        --cxx-compiler)
            CXX_COMPILER="$2"
            shift 2
            ;;
        --sysroot)
            if [[ $# -lt 2 || "${2:-}" == --* ]]; then
                echo "missing value for --sysroot" >&2
                exit 1
            fi
            SYSROOT="$2"
            shift 2
            ;;
        --c-flags)
            if [[ $# -lt 2 || "${2:-}" == --* ]]; then
                echo "missing value for --c-flags" >&2
                exit 1
            fi
            C_FLAGS="$2"
            shift 2
            ;;
        --cxx-flags)
            if [[ $# -lt 2 || "${2:-}" == --* ]]; then
                echo "missing value for --cxx-flags" >&2
                exit 1
            fi
            CXX_FLAGS="$2"
            shift 2
            ;;
        *)
            echo "unknown arg: $1" >&2
            exit 1
            ;;
    esac
done

if [[ -z "$PROJECT_PATH" ]]; then
    SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
    PROJECT_PATH="$(cd "${SCRIPT_DIR}/../.." && pwd)"
fi

PLATFORM_I2C_TOOLS_DIR="${PROJECT_PATH}/Platform/${PLATFORM}/3rdParty/i2c-tools"
ROOT_I2C_TOOLS_DIR="${PROJECT_PATH}/3rdParty/i2c-tools"
OUTPUT_BIN_DIR="${ROOT_I2C_TOOLS_DIR}/bin/${PLATFORM}"
I2C_TOOLS_VER="4.4"
TARBALL_NAME="i2c-tools-${I2C_TOOLS_VER}.tar.xz"
TARBALL_URL="https://www.kernel.org/pub/software/utils/i2c-tools/${TARBALL_NAME}"

TARBALL=""
for candidate in \
    "${PLATFORM_I2C_TOOLS_DIR}/${TARBALL_NAME}" \
    "${ROOT_I2C_TOOLS_DIR}/${TARBALL_NAME}"; do
    if [[ -f "$candidate" ]]; then
        TARBALL="$candidate"
        break
    fi
done

if [[ -z "$TARBALL" ]]; then
    mkdir -p "${ROOT_I2C_TOOLS_DIR}"
    echo "i2c-tools source not found, downloading: ${TARBALL_URL}"
    curl -L --fail --retry 3 --output "${ROOT_I2C_TOOLS_DIR}/${TARBALL_NAME}" "${TARBALL_URL}"
    TARBALL="${ROOT_I2C_TOOLS_DIR}/${TARBALL_NAME}"
fi

WORK_DIR="${PLATFORM_I2C_TOOLS_DIR}/.i2c-tools_build"
if [[ ! -d "${PLATFORM_I2C_TOOLS_DIR}" ]]; then
    WORK_DIR="${ROOT_I2C_TOOLS_DIR}/.i2c-tools_build_${PLATFORM}"
fi

rm -rf "$WORK_DIR"
mkdir -p "$WORK_DIR"

TAR_LIST_FILE="${WORK_DIR}/tar.list"
tar -tJf "$TARBALL" > "$TAR_LIST_FILE"
TOP_DIR_NAME="$(head -n 1 "$TAR_LIST_FILE" | cut -d'/' -f1)"
tar -xJf "$TARBALL" -C "$WORK_DIR"

SRC_DIR="${WORK_DIR}/${TOP_DIR_NAME}"
if [[ ! -d "$SRC_DIR" ]]; then
    echo "invalid tarball layout: ${TARBALL}" >&2
    exit 1
fi

CC="${C_COMPILER:-gcc}"
CXX="${CXX_COMPILER:-g++}"
AR="ar"
STRIP="strip"
if [[ -n "$C_COMPILER" ]]; then
    COMPILER_DIR="$(dirname "$C_COMPILER")"
    COMPILER_NAME="$(basename "$C_COMPILER")"
    if [[ "$COMPILER_NAME" == *-gcc ]]; then
        CROSS_PREFIX="${COMPILER_DIR}/${COMPILER_NAME%-gcc}"
        export PATH="${COMPILER_DIR}:${PATH}"
        AR="${CROSS_PREFIX}-ar"
        STRIP="${CROSS_PREFIX}-strip"
    fi
fi
export CC CXX

COMMON_CFLAGS="-O2 -Wall -fPIC"
if [[ -n "$C_FLAGS" ]]; then
    COMMON_CFLAGS="${COMMON_CFLAGS} ${C_FLAGS}"
fi
COMMON_LDFLAGS=""
if [[ -n "$SYSROOT" ]]; then
    COMMON_CFLAGS="${COMMON_CFLAGS} --sysroot=${SYSROOT}"
    COMMON_LDFLAGS="--sysroot=${SYSROOT}"
fi

(
    cd "$SRC_DIR"
    make -j"$(nproc)" \
        CC="$CC" \
        AR="$AR" \
        STRIP="$STRIP" \
        CFLAGS="$COMMON_CFLAGS" \
        LDFLAGS="$COMMON_LDFLAGS" \
        BUILD_DYNAMIC_LIB=0 \
        BUILD_STATIC_LIB=1 \
        USE_STATIC_LIB=1 \
        all-tools
)

TOOLS=(i2cdetect i2cdump i2cget i2cset i2ctransfer)
mkdir -p "$OUTPUT_BIN_DIR"
for tool in "${TOOLS[@]}"; do
    TOOL_BIN="${SRC_DIR}/tools/${tool}"
    if [[ ! -f "$TOOL_BIN" ]]; then
        echo "build success but executable not found: ${TOOL_BIN}" >&2
        exit 1
    fi
    cp -f "$TOOL_BIN" "${OUTPUT_BIN_DIR}/${tool}"
done

rm -rf "$WORK_DIR"
echo "i2c-tools build done: ${OUTPUT_BIN_DIR}"