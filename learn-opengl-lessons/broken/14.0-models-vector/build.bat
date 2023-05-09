@echo off
if not exist build mkdir build
xcopy /E /I /Y ..\libs\assimp\assimp*.dll build
xcopy /E /I /Y shaders build\shaders
xcopy /E /I /Y assets build\assets
pushd build
cl /nologo /Zi /MD ..\main.cpp /I..\..\libs\include /link ..\..\libs\glfw\lib-vc2019\glfw3.lib ..\..\libs\glew\glew32s.lib ..\..\libs\assimp\assimp.lib user32.lib gdi32.lib shell32.lib opengl32.lib 
popd
