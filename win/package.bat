@echo off

cmake --build build_rel --parallel 32 --config Release
if not %ERRORLEVEL% == 0 exit /B 1
@rd /S /Q pkg
mkdir pkg
copy build_rel\eco2d.exe pkg
rem tools\upx -9 pkg\eco2d.exe
robocopy art pkg\art /E
@del pkg\art\*.ecotex

IF NOT "%1"=="SKIP_DEPLOY" (
	pushd pkg
	..\tools\7za.exe a -r ..\eco2d.zip *.*
	popd

	butler push eco2d.zip zaklaus/eco2d:win64-latest
)
