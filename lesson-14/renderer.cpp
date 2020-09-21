#include "renderer.h"

#include <stdio.h>

void
Draw(Renderer *renderer, VertexArray *vertexArray, IndexBuffer *indexBuffer, Shader *shader)
{
    Bind(shader);
    Bind(vertexArray);
    Bind(indexBuffer);
    GLCall(glDrawElements(GL_TRIANGLES, indexBuffer->Count, GL_UNSIGNED_INT, NULL));
}

void
Clear(Renderer *renderer)
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

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