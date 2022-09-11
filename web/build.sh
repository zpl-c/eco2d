#!/bin/bash

set -xe

cmake --build build_web --parallel

pushd build_web
if [ -f "eco2d.html" ]; then
    mv eco2d.html index.html
fi
popd
