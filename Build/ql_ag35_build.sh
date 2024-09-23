#!/bin/bash
ENABLE_DEBUG="OFF"
BUILD_TYPE="Release"
PROJECT_PATH=$(pwd)/../
PROJECT_PLATFORM="ql_ag35"

if [[ $1 == $"DEBUG" ]]; then
    echo "This is debug version"
    ENABLE_DEBUG="ON"
fi

cd $PROJECT_PATH/Build/
rm -rf ../Release/Cache
rm -rf ../Release/Bin/*
rm -rf ../Release/Lib/*
mkdir -p ../Release/Cache/
cd ../Release/Cache/
cmake ../../ -DTEST_DEBUG=$ENABLE_DEBUG -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DPROJECT_PLATFORM=$PROJECT_PLATFORM
make -j24