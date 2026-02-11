#!/bin/bash

BUILD_TYPE="Debug"

while [[ $# -gt 0 ]]; do
    case $1 in
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

set -e

mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

make

echo "Build completed successfully!"
