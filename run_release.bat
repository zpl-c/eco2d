@echo off

cmake --build build --parallel 32 --config Release -- /nologo
build\Release\eco2d.exe %*
