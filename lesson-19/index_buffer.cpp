#include "index_buffer.h"
#include "renderer.h"
#include "GL/glew.h"

#include <malloc.h>

IndexBuffer *
NewIndexBuffer(const unsigned int *data, unsigned int count)
{
    ASSERT(sizeof(unsigned int) == sizeof(GLuint));

    IndexBuffer *Result = NULL;
    Result = (IndexBuffer *)malloc(sizeof(IndexBuffer));
    if (!Result)
    {
        return NULL;
    }

    Result->Count = count;
    GLCall(glGenBuffers(1, &Result->RendererID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Result->RendererID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));

    return Result;
}

void
DeleteIndexBuffer(IndexBuffer *i)
{
    GLCall(glDeleteBuffers(1, &i->RendererID));
}

void
Bind(IndexBuffer *i)
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i->RendererID));
}

void
Unbind(IndexBuffer *i)
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i->RendererID));
}