#!/bin/bash

set -xe

pushd build_web
if [ -f "index.html" ]; then
    rm -rf index.html
fi

APP=${1:-eco2d}

cmake --build . --parallel
if [ -f "$APP.html" ]; then
    mv $APP.html index.html
fi
popd
