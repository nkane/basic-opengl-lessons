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

    unsigned int vertex_shader_id;
    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &shader->vertex_file_buffer, NULL);
    glCompileShader(vertex_shader_id);
    int status_code;
    char info_log_buffer[512];
    unsigned int info_log_buffer_size = sizeof(info_log_buffer) / sizeof(char);
    memset(info_log_buffer, 0, info_log_buffer_size);
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &status_code);
    if (!status_code) 
    {
        glGetShaderInfoLog(vertex_shader_id, info_log_buffer_size, NULL, info_log_buffer);
        printf("[Error - Shader]: failed to compile vertex shader (%s), logs: %s\n", vertex_shader_path, info_log_buffer); 
        free(shader->vertex_file_buffer);
        free(shader->fragment_file_buffer);
        free(shader);
        return NULL;
    }

    unsigned int fragment_shader_id;
    fragment_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(fragment_shader_id, 1, &shader->vertex_file_buffer, NULL);
    glCompileShader(fragment_shader_id);
    status_code = 0;
    memset(info_log_buffer, 0, info_log_buffer_size);
    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &status_code);
    if (!status_code) 
    {
        glGetShaderInfoLog(fragment_shader_id, info_log_buffer_size, NULL, info_log_buffer);
        printf("[Error - Shader]: failed to compile fragment shader (%s), logs: %s\n", vertex_shader_path, info_log_buffer); 
        free(shader->vertex_file_buffer);
        free(shader->fragment_file_buffer);
        free(shader);
        return NULL;
    }

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return shader;
}

void
SetBoolUniform(ShaderProgram *s, const char *name, bool v)
{
    int id = glGetUniformLocation(s->id, name);
    glUniform1i(id, (int)v);
}

void
SetIntUniform(ShaderProgram *s, const char *name, int v)
{
    int id = glGetUniformLocation(s->id, name);
    glUniform1i(id, v);
}

void
SetFloatUniform(ShaderProgram *s, const char *name, float v)
{
    int id = glGetUniformLocation(s->id, name);
    glUniform1f(id, v);
}

void
SetFloatVec3Uniform(ShaderProgram *s, const char *name, glm::vec3 v)
{
    int id = glGetUniformLocation(s->id, name);
    glUniform3f(id, v.x, v.y, v.z);
}

void
SetFloat4Uniform(ShaderProgram *s, const char *name, float v[4])
{
    int id = glGetUniformLocation(s->id, name);
    glUniform4f(id, v[0], v[1], v[2], v[3]);
}

void
SetFloatMat4Uniform(ShaderProgram *s, const char *name, float *v)
{
    int id = glGetUniformLocation(s->id, name);
    glUniformMatrix4fv(id, 1, GL_FALSE, v);
}

void
ReleaseShaderProgram(ShaderProgram *s)
{
    if (s)
    {
        if (s->vertex_file_buffer)
        {
            free(s->vertex_file_buffer);
        }
        if (s->fragment_file_buffer)
        {
            free(s->fragment_file_buffer);
        }
        glDeleteProgram(s->id);
        free(s);
    }
}
#endif
