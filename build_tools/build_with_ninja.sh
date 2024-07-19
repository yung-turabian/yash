#!/bin/bash

set -e

BUILD_DIR=$1
if [ -z "$BUILD_DIR" ]; then
    echo "Usage: $0 <path-to-build-directory>"
    exit 1
fi

CURR_DIR=$(pwd)

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi


echo "Starting build..."
cd "$BUILD_DIR"
start_time=$(date +%s)
cmake -G Ninja ..
time ninja
end_time=$(date +%s)
cd "$CURR_DIR"

elapsed=$((end_time - start_time))
echo "Build completed in $elapsed seconds."

mv $BUILD_DIR/hrry $CURR_DIR

echo "Build completed successfully."
