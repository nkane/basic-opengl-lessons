#include "vertex_array.h"

VertexArray *
NewVertexArray()
{
    VertexArray *Result = NULL;
    Result = (VertexArray *)malloc(sizeof(VertexArray));
    if (Result == NULL)
    {
        return NULL;
    }

    GLCall(glGenVertexArrays(1, &Result->RendererID));
    GLCall(glBindVertexArray(Result->RendererID));

    return Result;
}

void
DeleteVertexArray(VertexArray *v)
{
    GLCall(glDeleteVertexArrays(1, &v->RendererID));
}

void
Bind(VertexArray *v)
{
    GLCall(glBindVertexArray(v->RendererID));
}

void
Unbind(VertexArray *v)
{
    GLCall(glBindVertexArray(0));
}

void 
AddBuffer(VertexArray *vertexArray, VertexBuffer *vertexBuffer, VertexBufferLayout *vertexBufferLayout)
{
    Bind(vertexArray);
    Bind(vertexBuffer);
    unsigned int offset = 0;
    for (int i = 0; i < vertexBufferLayout->Length; i++)
    {
        VertexBufferElement *element = &vertexBufferLayout->Elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i, element->Count, element->Type, 
                                     element->Normalized, vertexBufferLayout->Stride, (const void *)offset));
        offset += element->Count * element->TypeSize;
        element->Offset = offset;
    }
}