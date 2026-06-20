#!/usr/bin/env bash

set -euo pipefail

PROJECT_PATH=""
PLATFORM="Default"
C_COMPILER=""
SYSROOT=""
C_FLAGS=""

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
        --sysroot)
            SYSROOT="$2"
            shift 2
            ;;
        --c-flags)
            C_FLAGS="$2"
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

PLATFORM_SQLITE_DIR="${PROJECT_PATH}/Platform/${PLATFORM}/3rdParty/sqlite"
ROOT_SQLITE_DIR="${PROJECT_PATH}/3rdParty/sqlite"
OUTPUT_INCLUDE_DIR="${ROOT_SQLITE_DIR}/include"
OUTPUT_LIB_DIR="${ROOT_SQLITE_DIR}/lib/${PLATFORM}"

TARBALL="${PLATFORM_SQLITE_DIR}/sqlite-autoconf-3450300.tar.gz"
if [[ ! -f "$TARBALL" ]]; then
    TARBALL="${ROOT_SQLITE_DIR}/sqlite-autoconf-3450300.tar.gz"
fi

if [[ ! -f "$TARBALL" ]]; then
    echo "sqlite tarball not found for platform ${PLATFORM}" >&2
    exit 1
fi

WORK_DIR="${PLATFORM_SQLITE_DIR}/.sqlite_build"
if [[ ! -d "${PLATFORM_SQLITE_DIR}" ]]; then
    WORK_DIR="${ROOT_SQLITE_DIR}/.sqlite_build_${PLATFORM}"
fi

rm -rf "$WORK_DIR"
mkdir -p "$WORK_DIR"

TAR_LIST_FILE="${WORK_DIR}/tar.list"
tar -tzf "$TARBALL" > "$TAR_LIST_FILE"
TOP_DIR_NAME="$(head -n 1 "$TAR_LIST_FILE" | cut -d'/' -f1)"
tar -xzf "$TARBALL" -C "$WORK_DIR"

SRC_DIR="${WORK_DIR}/${TOP_DIR_NAME}"
if [[ ! -d "$SRC_DIR" ]]; then
    echo "invalid tarball layout: ${TARBALL}" >&2
    exit 1
fi

INSTALL_DIR="${WORK_DIR}/install"
mkdir -p "$INSTALL_DIR"

if [[ -n "$C_COMPILER" ]]; then
    export CC="$C_COMPILER"
fi

HOST_TRIPLET=""
if [[ -n "$C_COMPILER" ]]; then
    COMPILER_BASE="$(basename "$C_COMPILER")"
    HOST_TRIPLET="${COMPILER_BASE%-gcc}"
fi

if [[ -n "$SYSROOT" ]]; then
    C_FLAGS="${C_FLAGS} --sysroot=${SYSROOT}"
fi
if [[ -n "$C_FLAGS" ]]; then
    export CFLAGS="$C_FLAGS"
fi

CONFIGURE_ARGS=("--prefix=${INSTALL_DIR}" "--disable-shared" "--enable-static")
if [[ -n "$HOST_TRIPLET" ]]; then
    CONFIGURE_ARGS+=("--host=${HOST_TRIPLET}")
fi

(
    cd "$SRC_DIR"
    ./configure "${CONFIGURE_ARGS[@]}"
    make -j"$(nproc)"
    make install
)

SQLITE_LIB="${INSTALL_DIR}/lib/libsqlite3.a"
if [[ ! -f "$SQLITE_LIB" ]]; then
    echo "build success but libsqlite3.a not found" >&2
    exit 1
fi

mkdir -p "$OUTPUT_INCLUDE_DIR" "$OUTPUT_LIB_DIR"
cp -f "${INSTALL_DIR}/include/sqlite3.h" "$OUTPUT_INCLUDE_DIR/sqlite3.h"
cp -f "${INSTALL_DIR}/include/sqlite3ext.h" "$OUTPUT_INCLUDE_DIR/sqlite3ext.h"
cp -f "$SQLITE_LIB" "$OUTPUT_LIB_DIR/libsqlite3.a"

rm -rf "$WORK_DIR"
echo "sqlite build done: ${OUTPUT_LIB_DIR}"
