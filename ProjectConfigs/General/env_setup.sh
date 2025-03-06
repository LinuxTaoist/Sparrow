#!/bin/bash

SCRIPT_PATH=$(pwd)
RELEASE_PATH=$(dirname "$SCRIPT_PATH")
LIB_PATH="$RELEASE_PATH/Lib"

if [ -w "$BIN_PATH" ]; then
    chmod +x "$BIN_PATH"/*
else
    echo "Error: $BIN_PATH is not writable."
fi

export LD_LIBRARY_PATH="$LIB_PATH":"$LD_LIBRARY_PATH"

echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
