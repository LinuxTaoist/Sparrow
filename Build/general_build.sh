#!/bin/bash
ENABLE_DEBUG="OFF"
BUILD_TYPE="Release"
PROJECT_PATH=$(pwd)/..
PROJECT_PLATFORM="Default"
COMPILE_OPTIONS=${PROJECT_PATH}/Build/options/${PROJECT_PLATFORM}/${PROJECT_PLATFORM}_compile_options.txt

# Enable the build items option
ENABLE_BUILD_GTEST="ON"

if [[ $1 == $"DEBUG" ]]; then
    echo "This is debug version"
    ENABLE_DEBUG="ON"
fi

cd $PROJECT_PATH/Build/
mkdir -p ../Release/Cache
mkdir -p ../Release/Include

cd ../Release/Cache/
cmake ../../ \
    -DTEST_DEBUG=$ENABLE_DEBUG              \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE          \
    -DPROJECT_PLATFORM=$PROJECT_PLATFORM    \
    -DCMAKE_INSTALL_PREFIX=../              \
    -DBUILD_GTEST=$ENABLE_BUILD_GTEST       \
    -C $COMPILE_OPTIONS

make install -j16
