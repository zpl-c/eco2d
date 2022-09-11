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

# Build web
if [ -f "build_web/index.html" ]; then
    rm -rf build_web/index.html
fi

# Build the project
web/build.sh

# Package all assets
if [ ! -f "build_web/eco2d.html" ]; then
    echo "Build data not found. Compilation errors?"
    exit 1
fi

mkdir deploy_web
cp build_web/eco2d.* deploy_web/
cp buiid_web/index.html deploy_web/

# Deploy to itch.io
./butler/butler push deploy_web/ zaklaus/eco2d:html-latest

# Teardown
rm -rf deploy_web
