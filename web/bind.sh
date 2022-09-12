#!/bin/bash

# Symlink sources
if [ ! -d "build_web/map" ]; then
    pushd build_web
    mkdir -p "map/build_web"
    ln -s "/workspaces/eco2d/code" "map/code"
    ln -s "/workspaces/eco2d/build_web/_deps" "map/build_web/_deps"
    popd
fi
