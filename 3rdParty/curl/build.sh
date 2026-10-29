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

PLATFORM_CURL_DIR="${PROJECT_PATH}/Platform/${PLATFORM}/3rdParty/curl"
ROOT_CURL_DIR="${PROJECT_PATH}/3rdParty/curl"
OUTPUT_INCLUDE_DIR="${ROOT_CURL_DIR}/include"
OUTPUT_LIB_DIR="${ROOT_CURL_DIR}/lib/${PLATFORM}"
OUTPUT_BIN_DIR="${ROOT_CURL_DIR}/bin/${PLATFORM}"
CURL_VER="8.10.1"
TARBALL_NAME="curl-${CURL_VER}.tar.gz"
TARBALL_URL="https://curl.se/download/${TARBALL_NAME}"

TARBALL=""
for candidate in \
    "${PLATFORM_CURL_DIR}/${TARBALL_NAME}" \
    "${ROOT_CURL_DIR}/${TARBALL_NAME}"; do
    if [[ -f "$candidate" ]]; then
        TARBALL="$candidate"
        break
    fi
done

if [[ -z "$TARBALL" ]]; then
    mkdir -p "${ROOT_CURL_DIR}"
    echo "curl tarball not found, downloading: ${TARBALL_URL}"
    curl -L --fail --retry 3 --output "${ROOT_CURL_DIR}/${TARBALL_NAME}" "${TARBALL_URL}"
    TARBALL="${ROOT_CURL_DIR}/${TARBALL_NAME}"
fi

WORK_DIR="${PLATFORM_CURL_DIR}/.curl_build"
if [[ ! -d "${PLATFORM_CURL_DIR}" ]]; then
    WORK_DIR="${ROOT_CURL_DIR}/.curl_build_${PLATFORM}"
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

BUILD_DIR="${WORK_DIR}/build"
mkdir -p "$BUILD_DIR"
INSTALL_DIR="${WORK_DIR}/install"
mkdir -p "$INSTALL_DIR"

if [[ -n "$C_COMPILER" ]]; then
    export CC="$C_COMPILER"
fi
if [[ -n "$CXX_COMPILER" ]]; then
    export CXX="$CXX_COMPILER"
fi

HOST_TRIPLET=""
if [[ -n "$C_COMPILER" ]]; then
    COMPILER_BASE="$(basename "$C_COMPILER")"
    if [[ "$COMPILER_BASE" == *-gcc ]]; then
        HOST_TRIPLET="${COMPILER_BASE%-gcc}"
    fi
fi

COMMON_CFLAGS="-fPIC"
if [[ -n "$C_FLAGS" ]]; then
    COMMON_CFLAGS="${COMMON_CFLAGS} ${C_FLAGS}"
fi
if [[ -n "$SYSROOT" ]]; then
    COMMON_CFLAGS="${COMMON_CFLAGS} --sysroot=${SYSROOT}"
fi

if [[ -n "$CXX_FLAGS" ]]; then
    export CXXFLAGS="$CXX_FLAGS"
fi
export CFLAGS="$COMMON_CFLAGS"

CONFIGURE_ARGS=(
    "--prefix=${INSTALL_DIR}"
    "--disable-shared"
    "--enable-static"
    "--disable-docs"
    "--disable-manual"
    "--without-zlib"
    "--without-libpsl"
    "--without-libidn2"
    "--without-nghttp2"
    "--without-brotli"
    "--without-zstd"
    "--without-ssl"
    "--disable-ldap"
    "--disable-rtsp"
    "--disable-ftp"
    "--disable-file"
    "--disable-dict"
    "--disable-imap"
    "--disable-maildir"
    "--disable-mqtt"
    "--disable-pop3"
    "--disable-smbs"
    "--disable-smtp"
    "--disable-telnet"
    "--disable-tftp"
    "--disable-gopher"
)
if [[ -n "$HOST_TRIPLET" ]]; then
    CONFIGURE_ARGS+=("--host=${HOST_TRIPLET}")
fi

(
    cd "$SRC_DIR"
    ./configure "${CONFIGURE_ARGS[@]}"
    make -j"$(nproc)"
    make install
)

CURL_LIB="${INSTALL_DIR}/lib/libcurl.a"
if [[ ! -f "$CURL_LIB" ]]; then
    echo "build success but libcurl.a not found" >&2
    exit 1
fi

mkdir -p "$OUTPUT_INCLUDE_DIR" "$OUTPUT_LIB_DIR" "$OUTPUT_BIN_DIR"
cp -rf "${INSTALL_DIR}/include/curl" "$OUTPUT_INCLUDE_DIR/"
cp -f "$CURL_LIB" "$OUTPUT_LIB_DIR/libcurl.a"

CURL_BIN="${INSTALL_DIR}/bin/curl"
if [[ -f "$CURL_BIN" ]]; then
    cp -f "$CURL_BIN" "$OUTPUT_BIN_DIR/curl"
else
    echo "curl executable not found in install tree: ${CURL_BIN}" >&2
    exit 1
fi

rm -rf "$WORK_DIR"
echo "curl build done: ${OUTPUT_LIB_DIR} and ${OUTPUT_BIN_DIR}"
