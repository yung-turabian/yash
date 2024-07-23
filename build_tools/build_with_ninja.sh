#!/bin/bash

set -e

usage () {
  echo "Usage:"
  echo "   build_with_ninja [options]"
  echo ""
  echo "OPTIONS"
  echo "  -h | --help                 this help"
  echo "  -c | --clear                clears the screen"
  echo "  -t | --type                 change compile type, defaults to debug"
  exit 1
}
COMPILE_TYPE="debug"
CLEAN=${false}


DEFAULT_OPTIONS=0


if [ "$OPTIONS" != "" ] ; then
		set -- $OPTIONS "@@" "$@"
		DEFAULT_OPTIONS=1
fi

while [ "$#" -gt 0 ] ; do
		case $1 in
		
		-h | --help) 
				usage ;;

		-c | --clear)
				shift
				clear	
				shift ;;

		-t | --type)
				shift
				if [ "$#" -eq 0 ]; then echo "ERROR: Missing compile type"; usage ; fi
				COMPILE_TYPE=${1}
				shift ;;

		-cls | --clean)
				shift
				if [ "$#" -eq 0 ]; then echo "ERROR: Missing bool"; usage ; fi
				CLEAN=${1}
				shift ;;

		@@)
				DEFAULT_OPTIONS=0
				shift
				;;

		--* | -*) echo "Invalid option $1"; usage ;;

		esac
done

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


    echo "CMakePresets.json found in '$CURR_DIR'."
		
		PRESETS_DIR=$CURR_DIR


		echo "Starting build..."
		if [[ "$OSTYPE" == "linux-gnu"* ]]; then
				start_time=$(date +%s%3N)
		elif [[ "$OSTYPE" == "darwin"* ]]; then
				start_time=$(gdate +%s%3N)
		else
				SECONDS=0
				start_time=SECONDS
		fi

		cmake -S . -B ./build/ -G Ninja --preset "${COMPILE_TYPE}" &&
		cmake --build build/

		if [[ "$OSTYPE" == "linux-gnu"* ]]; then
				end_time=$(date +%s%3N)
		elif [[ "$OSTYPE" == "darwin"* ]]; then
				end_time=$(gdate +%s%3N)
		else
				end_time=SECONDS
		fi

		duration_ms=$((end_time - start_time))

		echo "Build completed: $duration_ms ms"

else 
		echo "CMakePresets.json not found in '$CURR_DIR'."
		echo "Returning..."
		exit 1
fi
