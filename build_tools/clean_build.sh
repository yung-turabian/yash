#!/bin/bash

BUILD_DIR=$1
if [ -z "$BUILD_DIR" ]; then
    echo "Usage: $0 <path-to-build-directory>"
    exit 1
fi

if [ -f "$BUILD_DIR/CMakeCache.txt" ]; then
    echo "CMakeCache.txt found in '$BUILD_DIR'."
		echo "Cleaning..."
		rm -rf $BUILD_DIR/*
else 
		echo "CMakeCache.txt not found in '$BUILD_DIR'."
		echo "Returning..."
		exit 1
fi

echo "build/ directory succesfully cleaned."
