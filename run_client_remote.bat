@echo off

call package.bat SKIP_DEPLOY

pkg\eco2d.exe -v %*
