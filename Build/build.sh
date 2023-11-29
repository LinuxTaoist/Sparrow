#!/bin/bash
ENABLE_DEBUG="OFF"

if [[ $1 == $"DEBUG" ]]; then
    echo "This is debug version"
    ENABLE_DEBUG="ON"
fi

rm -rf ../Release/Cache
rm -rf ../Release/Bin/*
rm -rf ../Release/Lib/*
mkdir -p ../Release/Cache/
cd ../Release/Cache/
cmake ../../ -DTEST_DEBUG=$ENABLE_DEBUG
make
