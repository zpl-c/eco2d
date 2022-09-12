#!/bin/bash

set -xe

pushd build_web
BUILD_PATH_PREFIX_MAP=$(pwd)=. cmake --build . --parallel
if [ -f "eco2d.html" ]; then
    mv eco2d.html index.html
fi
popd
