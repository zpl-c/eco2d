#!/bin/bash

set -xe

pushd build_web
if [ -f "index.html" ]; then
    rm -rf index.html
fi

cmake --build . --parallel
if [ -f "eco2d.html" ]; then
    mv eco2d.html index.html
fi
popd
