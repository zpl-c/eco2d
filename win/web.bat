rem set up VS (to get ninja generator)
call setup_cl_generic.bat amd64

rem clone emscripten sdk
if not exist "..\emsdk" (
	git clone https://github.com/emscripten-core/emsdk ..\emsdk
	pushd ..\emsdk
		call emsdk install  3.0.0 && rem latest
		call emsdk activate 3.0.0 && rem latest
	popd
)
if "%EMSDK%"=="" call ..\emsdk\emsdk_env.bat

rem host webserver, compile and launch
rem start "" python3 -m http.server --bind 127.0.0.1 8000 --directory build_web

pushd ..
    call emcmake cmake -S . -B build_web -DCMAKE_BUILD_TYPE=Release -DPLATFORM=Web
    call cmake --build build_web --parallel
    move build_web\eco2d.html build_web\index.html
popd
