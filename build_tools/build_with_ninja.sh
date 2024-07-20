#!/bin/bash

set -e

COMPILE_TYPE=${1:-'debug'} # defaults
CLEAN=${2:false}

#if [ -z "$BUILD_DIR" ]; then
#    echo "Usage: $0 <path-to-build-directory>"
# #   exit 1
#fi

if ! [ -d ./build ] ; then
		mkdir build
fi

if [ -f "./build/CMakeCache.txt" ] && [ $CLEAN ] ; then
		echo "Cleaning..."
		rm -rf ./build/*
fi

CURR_DIR=$(pwd)

if [ -f $CURR_DIR/CMakePresets.json ]; then
		
		# clear screen w/o reseting TERM
		echo -e "\033[2J\033[H"
		printf "\033[2J\033[H"

    echo "CMakePresets.json found in '$CURR_DIR'."
		
		PRESETS_DIR=$CURR_DIR


		echo "Starting build..."
		start_time=$(date +%s%3N)
		cmake -S . -B ./build/ -G Ninja --preset "${COMPILE_TYPE}" &&
		cmake --build build/
		end_time=$(date +%s%3N)

		elapsed_seconds=$(((end_time - start_time) / 1000))
		elapsed_ms=$(((end_time - start_time) % 1000))
		echo "Build completed: $elapsed_seconds.$elapsed_ms ms"

		mv ./build/hrry $CURR_DIR
else 
		echo "CMakePresets.json not found in '$CURR_DIR'."
		echo "Returning..."
		exit 1
fi
