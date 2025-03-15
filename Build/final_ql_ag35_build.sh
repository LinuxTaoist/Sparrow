#!/bin/bash
ENABLE_DEBUG="OFF"
BUILD_TYPE="Release"
PROJECT_PATH=$(pwd)/../
PROJECT_PLATFORM="ql_ag35"

# Enable the build items option
ENABLE_BUILD_GTEST="ON"

if [[ $1 == $"DEBUG" ]]; then
    echo "This is debug version"
    ENABLE_DEBUG="ON"
fi

cd $PROJECT_PATH/Build/
rm -rf ../Release/*
mkdir -p ../Release/Cache
mkdir -p ../Release/Include

cd ../Release/Cache/
cmake ../../ \
    -DTEST_DEBUG=$ENABLE_DEBUG              \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE          \
    -DPROJECT_PLATFORM=$PROJECT_PLATFORM    \
    -DCMAKE_INSTALL_PREFIX=../              \
    -DBUILD_GTEST=$ENABLE_BUILD_GTEST

make install -j24
