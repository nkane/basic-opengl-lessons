#ifndef _INTERAL_SHADER
#define _INTERAL_SHADER

#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#endif

#ifndef GLM_DEFINED
#define GLM_DEFINED
#include <glm/glm.hpp>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct _shader_program
{
    unsigned int id;
    char *vertex_file_buffer;
    char *fragment_file_buffer;
} ShaderProgram;

ShaderProgram *
CreateShaderProgram(const char *vertex_shader_path, const char *fragment_shader_path)
{
    FILE *vertex_shader_file = fopen(vertex_shader_path, "r");
    if (!vertex_shader_path)
    {
        return NULL;
    }
    FILE *fragment_shader_file = fopen(fragment_shader_path, "r");
    if (!fragment_shader_file)
    {
        fclose(vertex_shader_file);
        return NULL;
    }
    ShaderProgram *shader = (ShaderProgram *)malloc(sizeof(ShaderProgram));
    if (!shader)
    {
        fclose(vertex_shader_file);
        fclose(fragment_shader_file);
        return NULL;
    }
    unsigned int file_length;
    fseek(vertex_shader_file, 0L, SEEK_END);
    file_length = ftell(vertex_shader_file);
    fseek(vertex_shader_file, 0L, SEEK_SET);
    unsigned int buffer_size = (file_length + 1) * sizeof(char);
    shader->vertex_file_buffer = (char *)malloc(buffer_size);
    memset(shader->vertex_file_buffer, 0, buffer_size);
    fread(shader->vertex_file_buffer, sizeof(char), file_length, vertex_shader_file);
    fclose(vertex_shader_file);
    file_length = 0;
    fseek(fragment_shader_file, 0L, SEEK_END);
    file_length = ftell(fragment_shader_file);
    fseek(fragment_shader_file, 0L, SEEK_SET);
    buffer_size = 0;
    buffer_size = (file_length + 1) * sizeof(char);
    shader->fragment_file_buffer = (char *)malloc(buffer_size);
    memset(shader->fragment_file_buffer, 0, buffer_size);
    fread(shader->fragment_file_buffer, sizeof(char), file_length, fragment_shader_file);
    fclose(fragment_shader_file);

    //TODO(nick): finish implementing shader program, needs to actually generate shader!
    
    return shader;
}

#endif
