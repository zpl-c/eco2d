#!/bin/bash

set -xe

if [ -d "build_web" ]; then
    rm -rf build_web
fi

python -m http.server --directory build_web --bind 127.0.0.1
