cl /nologo /Zi /MD main.cpp ..\libs\include\imgui\*.cpp /I..\libs\include /link ..\libs\glfw\lib-vc2019\glfw3.lib ..\libs\glew\glew32s.lib user32.lib gdi32.lib shell32.lib opengl32.lib
::cl /nologo /Zi /MD ..\libs\include\imgui\*.cpp /I..\libs\include /link ..\libs\glfw\lib-vc2019\glfw3.lib ..\libs\glew\glew32s.lib user32.lib gdi32.lib shell32.lib opengl32.lib
::cp ..\\include\\imgui\\*.obj  .
::cl /nologo /Zi /MD main.cpp *.obj /I..\libs\include /link ..\libs\glfw\lib-vc2019\glfw3.lib ..\libs\glew\glew32s.lib user32.lib gdi32.lib shell32.lib opengl32.lib