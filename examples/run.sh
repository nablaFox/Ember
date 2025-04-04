#!/bin/bash

SCRIPT_PATH=$(dirname "$0")
BUILD_DIR="$SCRIPT_PATH/../build"

cmake --build "$BUILD_DIR"

cd "$BUILD_DIR" || exit 1

if [ -z "$1" ]; then
	echo "No demo specified. Please provide a demo name."
	exit 1
fi

if [ ! -f "$1" ]; then
	echo "Demo $1 not found in demos directory."
	exit 1
fi

./$1
