@echo off

rem build web first
call web.bat

if not %ERRORLEVEL% == 0 exit /B 1
@rd /S /Q pkg
mkdir pkg
copy ..\build_web\eco2d.* pkg
copy ..\build_web\index.html pkg

IF NOT "%1"=="SKIP_DEPLOY" (
	butler push pkg zaklaus/eco2d:html-latest
)
