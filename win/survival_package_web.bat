@echo off

rem build web first
call web.bat

if not %ERRORLEVEL% == 0 exit /B 1
@rd /S /Q pkg
mkdir pkg
copy ..\build_web\survival.* pkg
copy ..\build_web\survival.html pkg\index.html

IF NOT "%1"=="SKIP_DEPLOY" (
	butler push pkg zaklaus/chimera:html-latest
)
