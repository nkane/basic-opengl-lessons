@echo off
if not exist build mkdir build
pushd build
cl /nologo /Zi /MD ..\main.cpp /I..\..\..\libs\include /link ..\..\..\libs\glfw\lib-vc2019\glfw3.lib ..\..\..\libs\glew\glew32s.lib user32.lib gdi32.lib shell32.lib opengl32.lib 
popd