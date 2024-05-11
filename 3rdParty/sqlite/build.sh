#!/bin/bash

# Note: This script utilizes the system's default compilation toolchain.
# For cross-compilation, ensure the appropriate cross-compilation toolchain
# is configured beforehand.

# Check for existing SQLite source directory
if [[ -d "sqlite-autoconf-3450300" ]]; then
    echo "SQLite source directory exists."
else
    echo "SQLite source directory not found. Extracting..."
    tar -zxvf sqlite-autoconf-3450300.tar.gz
fi

# Enter the SQLite source directory
cd sqlite-autoconf-3450300

# Ensure the 'release' directory exists
if [[ ! -d "release" ]]; then
    echo "Creating release directory..."
    mkdir release
fi

# Configure, build, and install SQLite to the release directory
CURRENT_DIR=$(pwd)
./configure --host=arm-linux-gnueabihf --prefix="$CURRENT_DIR/release"
make
make install

# Copy the built SQLite libraries and headers to the ../include/ and ../lib/[platform]/
# cp release/include/* ../include/
# cp release/lib/*     ../lib/Default/

echo "SQLite compilation and installation completed."
