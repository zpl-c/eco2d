#!/bin/bash
# Run as "source web/host.sh" to set up environment variables

# Prompt for URL
read -p "Enter URL: " URL

# Expose URL to environment
export ECO2D_SERVER_URL=${URL/https:\/\//}run_web
echo ${ECO2D_SERVER_URL}

# Symlink sources
if [ ! -d "run_web/${ECO2D_SERVER_URL}" ]; then
    mkdir -p "run_web/${ECO2D_SERVER_URL/run_/build_}"
    pushd run_web
    ln -s "/workspaces/eco2d/code" "${ECO2D_SERVER_URL/run_web/}/code"
    ln -s "/workspaces/eco2d/build_web/_deps" "${ECO2D_SERVER_URL/run_web/}build_web/_deps"
    popd
fi
