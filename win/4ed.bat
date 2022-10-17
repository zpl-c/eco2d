@echo off
call %~dp0\setup_cl_generic.bat amd64
cls
pushd %~dp0\..
start "" 4ed .
popd