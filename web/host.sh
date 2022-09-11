#!/bin/bash

set -xe

if [ -d "run_web" ]; then
    rm -rf run_web
fi

mkdir run_web
cp build_web/eco2d.* run_web/
cp build_web/index.html run_web/

python -m http.server --directory run_web --bind 127.0.0.1
