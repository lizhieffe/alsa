#!/bin/bash
set -e

BUILD_DIR=build
SCRIPT_DIR=.
SRC_DIR=.

if [ ! -d "$BUILD_DIR" ]; then
  mkdir ${BUILD_DIR} 
fi
cd ${BUILD_DIR} 

echo "Generating Makefile..."
cmake ../${SRC_DIR}
echo

echo "Compiling..."
make
echo

echo "Running..."
./project
echo
