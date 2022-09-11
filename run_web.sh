#!/bin/bash

set -xe

python -m http.server --directory build_web --bind 127.0.0.1
