#!/bin/bash

set -xe

if [ ! -d "build_web" ]; then
    echo "Build directory not found. Run setup_web.sh first."
    exit 1
fi

if [ ! -d "butler" ]; then
    mkdir butler
    wget https://broth.itch.ovh/butler/linux-amd64/LATEST/archive/default -O butler.zip
    mv butler.zip butler/
    pushd butler/
        unzip butler.zip
        rm -rf butler.zip
        chmod +x ./butler
        ./butler -V
    popd
fi

# Build the project
web/build.sh

# Package all assets
if [ ! -f "build_web/index.html" ]; then
    echo "Build data not found. Compilation errors?"
    exit 1
fi

APP=${1:-eco2d}

mkdir -p deploy_web
cp build_web/$APP.* deploy_web/
cp build_web/index.html deploy_web/

# Deploy to itch.io
./butler/butler push deploy_web/ zaklaus/$APP:html-latest

# Teardown
rm -rf deploy_web
