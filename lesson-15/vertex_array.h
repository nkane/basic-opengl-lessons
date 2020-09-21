#pragma once

#include "vertex_buffer.h"
#include "renderer.h"
#include "GL/glew.h"

#include <malloc.h>

struct VertexArray
{
    unsigned int RendererID;
};

VertexArray *
NewVertexArray();

void
DeleteVertexArray(VertexArray *v);

void
Bind(VertexArray *v);

void
Unbind(VertexArray *v);

struct VertexBufferElement
{
    unsigned int Type;
    unsigned int Count;
    unsigned char Normalized;
    unsigned int TypeSize;
};

struct VertexBufferLayout
{
    VertexBufferElement *Elements;
    unsigned int Length;
    unsigned int Capacity;
    unsigned int Stride;
};

void 
AddBuffer(VertexArray *vertexArray, VertexBuffer *vertexBuffer, VertexBufferLayout *vertexBufferLayout);

VertexBufferLayout *
NewVertexBufferLayout(unsigned int elementLength)
{
    VertexBufferLayout *Result = NULL;
    Result = (VertexBufferLayout *)malloc(sizeof(VertexBufferLayout));
    if (Result == NULL)
    {
        return NULL;
    }

    Result->Elements = (VertexBufferElement *)calloc(elementLength, sizeof(VertexBufferElement));
    if (Result->Elements == NULL)
    {
        free(Result);
        return NULL;
    }
    Result->Capacity = elementLength;
    Result->Length = 0;
    Result->Stride = 0;

    return Result;
}

void
Push(VertexBufferLayout *b, unsigned int t, unsigned int n)
{
    unsigned int index = b->Length;
    if (b->Length >= b->Capacity)
    {
        unsigned int growSize = b->Capacity * 2;
        b->Elements = (VertexBufferElement *)realloc(b->Elements, growSize * sizeof(VertexBufferElement));
        ASSERT(b->Elements != NULL);
        b->Capacity= growSize;
        index++;
    }
    switch (t)
    {
        case GL_FLOAT:
        {
            b->Elements[index].TypeSize = sizeof(GLfloat);
            b->Elements[index].Normalized = GL_FALSE;
        } break;

        case GL_UNSIGNED_INT:
        {
            b->Elements[index].TypeSize = sizeof(GLuint);
            b->Elements[index].Normalized = GL_TRUE;
        } break;

        case GL_UNSIGNED_BYTE:
        {
            b->Elements[index].TypeSize = sizeof(GLubyte);
            b->Elements[index].Normalized = GL_TRUE;
        } break;

        default:
        {
            ASSERT(0);
        } break;
    }
    b->Elements[index].Type = t;
    b->Elements[index].Count = n;
    b->Stride += b->Elements[index].Count * b->Elements[index].TypeSize;
    b->Length++;
}