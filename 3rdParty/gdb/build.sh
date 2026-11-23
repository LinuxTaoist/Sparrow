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

PLATFORM_GDB_DIR="${PROJECT_PATH}/Platform/${PLATFORM}/3rdParty/gdb"
ROOT_GDB_DIR="${PROJECT_PATH}/3rdParty/gdb"
OUTPUT_BIN_DIR="${ROOT_GDB_DIR}/bin/${PLATFORM}"
GDB_VER="10.2"
TARBALL_NAME="gdb-${GDB_VER}.tar.xz"
TARBALL_URL="https://ftp.gnu.org/gnu/gdb/${TARBALL_NAME}"

TARBALL=""
for candidate in \
    "${PLATFORM_GDB_DIR}/${TARBALL_NAME}" \
    "${ROOT_GDB_DIR}/${TARBALL_NAME}"; do
    if [[ -f "$candidate" ]]; then
        TARBALL="$candidate"
        break
    fi
done

if [[ -z "$TARBALL" ]]; then
    mkdir -p "${ROOT_GDB_DIR}"
    echo "gdb source not found, downloading: ${TARBALL_URL}"
    curl -L --fail --retry 3 --output "${ROOT_GDB_DIR}/${TARBALL_NAME}" "${TARBALL_URL}"
    TARBALL="${ROOT_GDB_DIR}/${TARBALL_NAME}"
fi

WORK_DIR="${PLATFORM_GDB_DIR}/.gdb_build"
if [[ ! -d "${PLATFORM_GDB_DIR}" ]]; then
    WORK_DIR="${ROOT_GDB_DIR}/.gdb_build_${PLATFORM}"
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

BUILD_DIR="${WORK_DIR}/build"
mkdir -p "$BUILD_DIR"

CC="${C_COMPILER:-gcc}"
CXX="${CXX_COMPILER:-g++}"
export CC CXX
TARGET_TRIPLET="$("$CC" -dumpmachine)"
BUILD_TRIPLET="$(bash "${SRC_DIR}/config.guess")"

if [[ -n "$C_COMPILER" ]]; then
    COMPILER_DIR="$(dirname "$C_COMPILER")"
    COMPILER_NAME="$(basename "$C_COMPILER")"
    if [[ "$COMPILER_NAME" == *-gcc ]]; then
        CROSS_PREFIX="${COMPILER_DIR}/${COMPILER_NAME%-gcc}"
        export PATH="${COMPILER_DIR}:${PATH}"
        export AR="${CROSS_PREFIX}-ar"
        export AS="${CROSS_PREFIX}-as"
        export LD="${CROSS_PREFIX}-ld"
        export NM="${CROSS_PREFIX}-nm"
        export OBJCOPY="${CROSS_PREFIX}-objcopy"
        export OBJDUMP="${CROSS_PREFIX}-objdump"
        export RANLIB="${CROSS_PREFIX}-ranlib"
        export STRIP="${CROSS_PREFIX}-strip"
    fi
fi

COMMON_CFLAGS="-O2 -fPIC"
if [[ -n "$C_FLAGS" ]]; then
    COMMON_CFLAGS="${COMMON_CFLAGS} ${C_FLAGS}"
fi
COMMON_CXXFLAGS="-O2 -fPIC"
if [[ -n "$CXX_FLAGS" ]]; then
    COMMON_CXXFLAGS="${COMMON_CXXFLAGS} ${CXX_FLAGS}"
fi
COMMON_LDFLAGS=""
if [[ -n "$SYSROOT" ]]; then
    COMMON_CFLAGS="${COMMON_CFLAGS} --sysroot=${SYSROOT}"
    COMMON_CXXFLAGS="${COMMON_CXXFLAGS} --sysroot=${SYSROOT}"
    COMMON_LDFLAGS="${COMMON_LDFLAGS} --sysroot=${SYSROOT}"
fi
export CFLAGS="$COMMON_CFLAGS"
export CXXFLAGS="$COMMON_CXXFLAGS"
export LDFLAGS="$COMMON_LDFLAGS"

CONFIGURE_ARGS=(
    "--build=${BUILD_TRIPLET}"
    "--host=${TARGET_TRIPLET}"
    "--target=${TARGET_TRIPLET}"
    "--prefix=/usr"
    "--disable-nls"
    "--disable-tui"
    "--disable-sim"
    "--with-expat=no"
    "--with-mpfr=no"
    "--with-python=no"
    "--with-guile=no"
    "--with-lzma=no"
    "--with-babeltrace=no"
)

(
    cd "$BUILD_DIR"
    "${SRC_DIR}/configure" "${CONFIGURE_ARGS[@]}"
    make -j"$(nproc)" MAKEINFO=true all-gdb
)

GDB_BIN="${BUILD_DIR}/gdb/gdb"
if [[ ! -f "$GDB_BIN" ]]; then
    echo "build success but gdb executable not found: ${GDB_BIN}" >&2
    exit 1
fi

mkdir -p "$OUTPUT_BIN_DIR"
cp -f "$GDB_BIN" "$OUTPUT_BIN_DIR/gdb"

rm -rf "$WORK_DIR"
echo "gdb build done: ${OUTPUT_BIN_DIR}/gdb"