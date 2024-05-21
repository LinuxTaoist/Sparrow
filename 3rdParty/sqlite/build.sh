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

# Check for existing sqlite source directory
if [[ ! -d "sqlite-autoconf-3450300" ]]; then
    echo "sqlite source directory not found. Extracting..."
    tar -zxvf sqlite-autoconf-3450300.tar.gz
fi

# Enter the sqlite source directory
cd sqlite-autoconf-3450300

# Ensure the 'release' directory exists
if [[ ! -d "release" ]]; then
    echo "Creating release directory..."
    mkdir release
fi

# Configure, build, and install sqlite to the release directory
CURRENT_DIR=$(pwd)
./configure --host=arm-linux-gnueabihf --prefix="$CURRENT_DIR/release"
make
make install

# Copy the built sqlite libraries and headers to the ../include/ and ../lib/[platform]/
cp -rf release/include/*    ../include/
cp -rf release/lib/*        ../lib/$PLATFORM/

echo "sqlite compilation and installation completed."
