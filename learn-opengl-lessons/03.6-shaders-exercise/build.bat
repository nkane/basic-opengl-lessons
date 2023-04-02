@echo off
if not exist build mkdir build
if not exist build\shaders mkdir build\shaders
copy /y shaders build\shaders
pushd build
cl /nologo /Zi /MD ..\main.cpp /I..\..\libs\include /link ..\..\libs\glfw\lib-vc2019\glfw3.lib ..\..\libs\glew\glew32s.lib user32.lib gdi32.lib shell32.lib opengl32.lib 
popd