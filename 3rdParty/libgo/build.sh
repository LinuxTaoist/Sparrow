#!/bin/bash

# Note: This script utilizes the system's default compilation toolchain.
# For cross-compilation, ensure the appropriate cross-compilation toolchain
# is configured beforehand.

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
# cp release/include/libgo/*    ../include/
# cp release/lib/*              ../lib/Default/

echo "libgo compilation and installation completed."
