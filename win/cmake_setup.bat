@echo off

REM desktop
cmake -B build -DCMAKE_BUILD_TYPE=Debug -G Ninja
cmake -B build_rel -DCMAKE_BUILD_TYPE=Release -G Ninja
