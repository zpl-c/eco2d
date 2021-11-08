@echo off

call package.bat SKIP_DEPLOY
cls
pkg\eco2d.exe -d %*
