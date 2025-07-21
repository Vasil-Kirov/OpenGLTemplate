@ECHO OFF
setlocal enabledelayedexpansion

set file_array=main.c glad.c asset_loader.c memory.c renderer.c log.c gl\shader.c gl\buffer.c gl\texture.c gl\framebuffer.c

set files=

for %%f in (%file_array%) do (
	set files=!files!../src/%%f 
)


pushd bin

clang %files% -I..\include -IC:\SDL3\include C:\SDL3\bin\Release\*.lib --debug

popd


