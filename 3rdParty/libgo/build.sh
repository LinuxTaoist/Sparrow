#!/bin/bash

# Note: This script utilizes the system's default compilation toolchain.
# For cross-compilation, ensure the appropriate cross-compilation toolchain
# is configured beforehand.

# platform
PLATFORM="Default"

# Ensure the include directory exists
if [[ ! -d "include" ]]; then
    echo "Creating include directory..."
    mkdir "include"
fi

# Ensure the lib/platform directory exists
if [[ ! -d "lib/$PLATFORM" ]]; then
    echo "Creating $PLATFORM directory..."
    mkdir -p "lib/$PLATFORM"
fi

# Check for existing libgo source directory
if [[ ! -d "libgo" ]]; then
    echo "libgo source directory not found. Extracting..."
    tar -zxvf libgo.tar.gz
fi

# Enter the libgo source directory
cd libgo

# Ensure the 'release' directory exists
if [[ ! -d "release" ]]; then
    echo "Creating release directory..."
    mkdir release
fi

CURRENT_DIR=$(pwd)
cd release
cmake -DCMAKE_INSTALL_PREFIX="$CURRENT_DIR/release" ..
make
make install

# Copy the built libgo libraries and headers to the ../include/ and ../lib/[platform]/
cd ../
cp -rf release/include/libgo/*    ../include/
cp -rf release/lib/*              ../lib/$PLATFORM/

echo "libgo compilation and installation completed."
