#include "vertex_buffer.h"
#include "renderer.h"

#include <malloc.h>

VertexBuffer *
NewVertexBuffer(const void *data, unsigned int size)
{
    VertexBuffer *Result = NULL;
    Result = (VertexBuffer *)malloc(sizeof(VertexBuffer));
    if (Result == NULL)
    {
        return Result;
    }

    GLCall(glGenBuffers(1, &Result->RendererID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, Result->RendererID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW)); 

    return Result;
}

void
DeleteVertexBuffer(VertexBuffer *v)
{
    GLCall(glDeleteBuffers(1, &v->RendererID));
}

void
Bind(VertexBuffer *v)
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, v->RendererID));
}

void
Unbind(VertexBuffer *v)
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}