#!/bin/bash
BUILD_TYPE="Release"
PROJECT_PATH=$(pwd)/../
PROJECT_PLATFORM="bw_imx6ull"

cd $PROJECT_PATH/Build/
mkdir -p ../Release/Cache
mkdir -p ../Release/Include

cd ../Release/Cache/
cmake ../../ \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE          \
    -DPROJECT_PLATFORM=$PROJECT_PLATFORM    \
    -DCMAKE_INSTALL_PREFIX=../

make install -j24
