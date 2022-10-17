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
start python -m http.server --bind 127.0.0.1 8000 --directory build_web

pushd ..
    emcmake cmake -S . -B build_web -DCMAKE_BUILD_TYPE=Release -DPLATFORM=Web
    cmake --build build_web --parallel
    move build_web\eco2d.html build_web\index.html
popd
