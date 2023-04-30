#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define GLEW_STATIC
#include <GL\glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef struct _shader_program
{
    unsigned int id;
    char *vertex_file_buffer;
    char *fragment_file_buffer;
} ShaderProgram;

ShaderProgram *
CreateShaderProgram(const char *vertex_shader_path, const char *fragment_shader_path)
{
    ShaderProgram *Result = (ShaderProgram *)malloc(sizeof(ShaderProgram));
    if (!Result)
    {
        return NULL;
    }

    FILE *vertex_shader_file = fopen(vertex_shader_path, "r");
    if (!vertex_shader_path)
    {
        free(Result);
        return NULL;
    }
    unsigned int file_length = 0;
    fseek(vertex_shader_file, 0L, SEEK_END);
    file_length = ftell(vertex_shader_file);
    fseek(vertex_shader_file, 0L, SEEK_SET);
    Result->vertex_file_buffer = (char *)malloc(file_length * sizeof(char) + 1);
    memset(Result->vertex_file_buffer, 0, file_length + 1);
    fread(Result->vertex_file_buffer, sizeof(char), file_length, vertex_shader_file);
    file_length = 0;

    FILE *fragment_shader_file = fopen(fragment_shader_path, "r");
    if (!fragment_shader_file)
    {
        fclose(vertex_shader_file);
        free(Result->vertex_file_buffer);
        free(Result);
        return NULL;
    }
    fseek(fragment_shader_file, 0L, SEEK_END);
    file_length = ftell(fragment_shader_file);
    fseek(fragment_shader_file, 0L, SEEK_SET);
    Result->fragment_file_buffer = (char *)malloc(file_length * sizeof(char) + 1);
    memset(Result->fragment_file_buffer, 0, file_length + 1);
    fread(Result->fragment_file_buffer, sizeof(char), file_length, fragment_shader_file);

    fclose(vertex_shader_file);
    fclose(fragment_shader_file);

    unsigned int vertex_shader_id;
    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &Result->vertex_file_buffer, NULL);
    glCompileShader(vertex_shader_id);
    int success;
    char info_logs[512];
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader_id, 512, NULL, info_logs);
        printf("[Error - Vertex Shader]: %s\n", info_logs);
        free(Result->vertex_file_buffer);
        free(Result->fragment_file_buffer);
        // TODO(nick): release stuff
        return NULL;
    }
    memset(info_logs, 0, 512);

    unsigned int fragment_shader_id;
    fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &Result->fragment_file_buffer, NULL);
    glCompileShader(fragment_shader_id);
    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader_id, 512, NULL, info_logs);
        printf("[Error - Fragment Shader]: %s\n", info_logs);
        // TODO(nick): release stuff
        return NULL;
    }
    memset(info_logs, 0, 512);

    // create shader program linking vertex and fragment shader
    Result->id = glCreateProgram();
    glAttachShader(Result->id, vertex_shader_id);
    glAttachShader(Result->id, fragment_shader_id);
    glLinkProgram(Result->id);
    glGetProgramiv(Result->id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(Result->id, 512, NULL, info_logs);
        printf("[Error - Program Linking]: %s\n", info_logs);
    }
    memset(info_logs, 0, 512);

    // shaders no longer needed, since linked to program
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return Result;
}

void
SetBoolUniform(ShaderProgram *shader_program, const char *name, bool value)
{
    int uniform_id = glGetUniformLocation(shader_program->id, name);
    glUniform1i(uniform_id, (int)value);
}

void
SetIntUniform(ShaderProgram *shader_program, const char *name, int value)
{
    int uniform_id = glGetUniformLocation(shader_program->id, name);
    glUniform1i(uniform_id, value);
}

void
SetFloatUniform(ShaderProgram *shader_program, const char *name, float value)
{
    int uniform_id = glGetUniformLocation(shader_program->id, name);
    glUniform1f(uniform_id, value);
}

void
SetFloatVec3Uniform(ShaderProgram *shader_program, const char *name, glm::vec3 value)
{
    int uniform_id = glGetUniformLocation(shader_program->id, name);
    glUniform3f(uniform_id, value.x, value.y, value.z);
}

void
SetFloat4Uniform(ShaderProgram *shader_program, const char *name, float value[4])
{
    int uniform_id = glGetUniformLocation(shader_program->id, name);
    glUniform4f(uniform_id, value[0], value[1], value[2], value[3]);
}

void
SetFloatMat4Uniform(ShaderProgram *shader_program, const char *name, float *value)
{
    int uniform_id = glGetUniformLocation(shader_program->id, name);
    glUniformMatrix4fv(uniform_id, 1, GL_FALSE, value);
}

void
ReleaseShaderProgram(ShaderProgram *shader_program)
{
    if (shader_program)
    {
        if (shader_program->vertex_file_buffer)
        {
            free(shader_program->vertex_file_buffer);
        }
        if (shader_program->fragment_file_buffer)
        {
            free(shader_program->fragment_file_buffer);
        }
        glDeleteProgram(shader_program->id);
        free(shader_program);
    }
}
