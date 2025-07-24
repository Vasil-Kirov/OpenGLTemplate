@ECHO OFF
setlocal enabledelayedexpansion

set file_array=main.c glad.c asset_loader.c string8.c memory.c renderer.c log.c gl\shader.c gl\buffer.c gl\texture.c gl\framebuffer.c

set files=

for %%f in (%file_array%) do (
	set files=!files!../src/%%f 
)

set SDL_LOCATION=C:\SDL3
set SDL_BIN=%SDL_LOCATION%\bin\Release

pushd bin

clang %files% -I..\include -I%SDL_LOCATION%\include %SDL_BIN%\*.lib ..\libs\*.lib --debug

popd


