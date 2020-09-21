#include "renderer.h"

#include <stdio.h>

void
GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool 
GLLogCall(const char *function, const char *file, int line)
{
    while (GLenum error = glGetError())
    {
        printf("[OpenGL Error] : (%u : 0x%x) - %s %s:%d\n", error, error, function, file, line);
        return false;
    }
    return true; 
}