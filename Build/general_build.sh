#!/bin/bash
ENABLE_DEBUG="OFF"
PROJECT_PATH=$(pwd)/../

if [[ $1 == $"DEBUG" ]]; then
    echo "This is debug version"
    ENABLE_DEBUG="ON"
fi

## build libgo
LIBGO_LIBRARY_PATH=$PROJECT_PATH/3rdParty/libgo/build/liblibgo.a
if [ -f "$LIBGO_LIBRARY_PATH" ]; then
    echo "libgo.a already exists"
else
    echo "libgo.a not exists, start build libgo"
    mkdir -p ../3rdParty/libgo/build
    cd ../3rdParty/libgo/build
    cmake ..
    make
fi

cd $PROJECT_PATH/Build/
rm -rf ../Release/Cache
rm -rf ../Release/Bin/*
rm -rf ../Release/Lib/*
mkdir -p ../Release/Cache/
cd ../Release/Cache/
cmake ../../ -DTEST_DEBUG=$ENABLE_DEBUG
make
