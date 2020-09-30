#include "shader.h"
#include "renderer.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

Shader *
NewShader(const char *vertexSourcePath, unsigned int n1, const char *fragmentSourcePath, unsigned int n2)
{
    Shader *Result = NULL;
    Result = (Shader *)malloc(sizeof(Shader));
    if (Result == NULL)
    {
        return NULL;
    }

    Result->VertexSourceFilePath = (char *)malloc(sizeof(char) * n1);
    ASSERT(Result->VertexSourceFilePath != NULL);
    strcpy(Result->VertexSourceFilePath, vertexSourcePath);

    Result->FragmentSourceFilePath = (char *)malloc(sizeof(char) * n2);
    ASSERT(Result->FragmentSourceFilePath != NULL);
    strcpy(Result->FragmentSourceFilePath, fragmentSourcePath);

    Result->VertexShader = ParseShader("shaders/vertex.glsl");
    ASSERT(Result->VertexShader != NULL);
    Result->FragmentShader = ParseShader("shaders/fragment.glsl");
    ASSERT(Result->FragmentShader != NULL);

    Result->RendererID = CreateShader(Result->VertexShader, Result->FragmentShader);

    Result->UniformCache = (UniformCache *)calloc(256, sizeof(UniformCache));
    ASSERT(Result->UniformCache != NULL);

    memset(Result->UniformCache, 0, 256 * sizeof(UniformCache));

    return Result;
}

void
DeleteShader(Shader *s)
{
    GLCall(glDeleteProgram(s->RendererID));
    free(s->VertexSourceFilePath);
    free(s->FragmentSourceFilePath);
    free(s->VertexShader);
    free(s->FragmentShader);
}

unsigned int
CompileShader(Shader *s)
{
    return false;
}

char *
ParseShader(const char *filePath)
{
    FILE *fileHandle = fopen(filePath, "r");
    if (!fileHandle)
    {
        printf("Failed to load file: %s\n", filePath);
        return NULL;
    }

    char *shaderBuffer = (char *)malloc(1024 * sizeof(char));
    memset(shaderBuffer, 0, 1024 * sizeof(char));
    char lineBuffer[256] = {};

    while(fgets(lineBuffer, 256, fileHandle))
    {
        strcat(shaderBuffer, lineBuffer);
    }

    fclose(fileHandle);
    return shaderBuffer;
}

unsigned int
CompilerShader(unsigned int type, const char *source)
{
    GLCall(unsigned int id = glCreateShader(type)); 
    GLCall(glShaderSource(id, 1, &source, NULL));
    GLCall(glCompileShader(id));
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char *message = (char *)_alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        printf("OpenGL Error Compiling Shader: %s\n", message);
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

unsigned int
CreateShader(const char *vertexShader, const char *fragmentShader)
{
    GLCall(unsigned int program = glCreateProgram());
    GLCall(unsigned int vs = CompilerShader(GL_VERTEX_SHADER, vertexShader));
    GLCall(unsigned int fs = CompilerShader(GL_FRAGMENT_SHADER, fragmentShader));

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

void 
Bind(Shader *s)
{
    GLCall(glUseProgram(s->RendererID));
}

void 
Unbind(Shader *s)
{
    GLCall(glUseProgram(0));
}

void
SetUniform1i(Shader *s, const char *name, int v)
{
    int uniformLocation = GetUniformLocation(s, name);
    GLCall(glUniform1i(uniformLocation, v));
}

void
SetUniform1f(Shader *s, const char *name, float v)
{
    int uniformLocation = GetUniformLocation(s, name);
    GLCall(glUniform1f(uniformLocation, v));
}

void
SetUniform4f(Shader *s, const char *name, float v[4])
{
    int uniformLocation = GetUniformLocation(s, name);
    GLCall(glUniform4f(uniformLocation, v[0], v[1], v[2], v[3]));
}

void
SetUniformMatrix4f(Shader *s, const char *name, glm::mat4 matrix)
{
    int uniformLocation = GetUniformLocation(s, name);
    GLCall(glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &matrix[0][0]));
}

int
GetUniformLocation(Shader *s, const char *name)
{
    unsigned int cacheIndex = hash(name);
    if (s->UniformCache[cacheIndex].Name != NULL && strcmp(s->UniformCache[cacheIndex].Name, name) == 0)
    {
        return s->UniformCache[cacheIndex].ID;
    }
    GLCall(int uniformLocation = glGetUniformLocation(s->RendererID, name));
    if (uniformLocation == -1)
    {
        printf("Warning: uniform %s doesn't exist", name);
    }
    s->UniformCache[cacheIndex].ID = uniformLocation;
    unsigned int size = strlen(name) + 1;
    s->UniformCache[cacheIndex].Name = (char *)malloc(size * sizeof(char));
    memset(s->UniformCache, 0, size * sizeof(char));
    strcpy(s->UniformCache[cacheIndex].Name, name);
    return uniformLocation;
}