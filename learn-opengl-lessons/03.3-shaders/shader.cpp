#include <stdio.h>
#include <stdlib.h>

typedef struct _shader_program
{
    unsigned int program_id;
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
    Result->vertex_file_buffer[file_length + 1] = '\0';
    fread(Result->vertex_file_buffer, sizeof(char), file_length, vertex_shader_file);

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
    Result->fragment_file_buffer[file_length + 1] = '\0';
    fread(Result->fragment_file_buffer, sizeof(char), file_length, fragment_shader_file);

    fclose(vertex_shader_file);
    fclose(fragment_shader_file);

    return Result;
}

void
ReleaseShaderProgram(ShaderProgram *shader_program)
{

}