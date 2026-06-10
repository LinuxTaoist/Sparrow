#!/usr/bin/env bash

set -euo pipefail

PROJECT_PATH=""
PLATFORM=""
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
            SYSROOT="$2"
            shift 2
            ;;
        --c-flags)
            C_FLAGS="$2"
            shift 2
            ;;
        --cxx-flags)
            CXX_FLAGS="$2"
            shift 2
            ;;
        *)
            echo "unknown arg: $1" >&2
            exit 1
            ;;
    esac
done

if [[ -z "$PROJECT_PATH" || -z "$PLATFORM" ]]; then
    echo "usage: build.sh --project-path <path> --platform <platform> [--c-compiler <path>] [--cxx-compiler <path>] [--sysroot <path>] [--c-flags <flags>] [--cxx-flags <flags>]" >&2
    exit 1
fi

PLATFORM_GTEST_DIR="${PROJECT_PATH}/Platform/${PLATFORM}/3rdParty/googletest"
ROOT_GTEST_DIR="${PROJECT_PATH}/3rdParty/googletest"
OUTPUT_LIB_DIR="${ROOT_GTEST_DIR}/lib/${PLATFORM}"

TARBALL="${PLATFORM_GTEST_DIR}/googletest-1.5.0.tar.gz"
if [[ ! -f "$TARBALL" ]]; then
    TARBALL="${ROOT_GTEST_DIR}/googletest-1.5.0.tar.gz"
fi

if [[ ! -f "$TARBALL" ]]; then
    echo "googletest tarball not found for platform ${PLATFORM}" >&2
    exit 1
fi

WORK_DIR="${PLATFORM_GTEST_DIR}/.gtest_build"
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

BUILD_DIR="${WORK_DIR}/build"
mkdir -p "$BUILD_DIR"

CMAKE_ARGS=("$SRC_DIR")
if [[ -n "$C_COMPILER" ]]; then
    CMAKE_ARGS+=("-DCMAKE_C_COMPILER=${C_COMPILER}")
fi
if [[ -n "$CXX_COMPILER" ]]; then
    CMAKE_ARGS+=("-DCMAKE_CXX_COMPILER=${CXX_COMPILER}")
fi
if [[ -n "$SYSROOT" ]]; then
    CMAKE_ARGS+=("-DCMAKE_SYSROOT=${SYSROOT}")
fi

if [[ -n "$C_FLAGS" ]]; then
    export CFLAGS="$C_FLAGS"
fi
if [[ -n "$CXX_FLAGS" ]]; then
    export CXXFLAGS="$CXX_FLAGS"
fi

(
    cd "$BUILD_DIR"
    cmake "${CMAKE_ARGS[@]}"
    make -j"$(nproc)"
)

GTEST_LIB="$(find "$BUILD_DIR" -name libgtest.a -print -quit || true)"
GTEST_MAIN_LIB="$(find "$BUILD_DIR" -name libgtest_main.a -print -quit || true)"

if [[ -z "$GTEST_LIB" || -z "$GTEST_MAIN_LIB" ]]; then
    echo "build success but gtest static libs not found" >&2
    exit 1
fi

mkdir -p "$OUTPUT_LIB_DIR"
cp -f "$GTEST_LIB" "$OUTPUT_LIB_DIR/libgtest.a"
cp -f "$GTEST_MAIN_LIB" "$OUTPUT_LIB_DIR/libgtest_main.a"

rm -rf "$WORK_DIR"
echo "gtest build done: ${OUTPUT_LIB_DIR}"
