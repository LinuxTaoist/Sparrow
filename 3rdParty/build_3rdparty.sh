#!/bin/bash

## build libgon2
cd libgo
./build.sh
cd ..

## build sqlite
cd sqlite
./build.sh

