#pragma once

#include "GL/glew.h"

struct UniformCache
{
    int ID;
    char *Name;
};

unsigned int
hash(const char *str)
{
    unsigned long hash = 5381;
    int c = 0;
    while (c = *str++) 
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % 256;
}

struct Shader
{
    unsigned int RendererID;
    char* VertexSourceFilePath;
    char* FragmentSourceFilePath;
    char *VertexShader;
    char *FragmentShader;
    UniformCache *UniformCache;
};

Shader *
NewShader(const char *vertexSourcePath, unsigned int n1, const char *fragmentSourcePath, unsigned int n2);

void
DeleteShader(Shader *s);

unsigned int
CompilerShader(unsigned int type, const char *source);

char *
ParseShader(const char *filePath);

unsigned int
CreateShader(const char *vertexShader, const char *fragmentShader);

void 
Bind(Shader *s);

void 
Unbind(Shader *s);

void
SetUniform4f(Shader *s, const char *name, float v[4]);

int
GetUniformLocation(Shader *s, const char *name);
