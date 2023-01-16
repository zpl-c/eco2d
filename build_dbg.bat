@echo off
call win\setup_cl_generic.bat amd64
cmake --build build && build\eco2d
