#pragma once

#include <GL/glew.h>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

#include "vertex_array.h"
#include "index_buffer.h"
#include "shader.h"

struct Renderer
{

};

void
Draw(Renderer *renderer, VertexArray *vertexArray, IndexBuffer *indexBuffer, Shader *shader);

void
Clear(Renderer *renderer);

void
GLClearError();

bool 
GLLogCall(const char *function, const char *file, int line);