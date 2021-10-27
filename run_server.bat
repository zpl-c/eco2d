    @echo off

call package.bat
cls
pkg\eco2d.exe -d %*
