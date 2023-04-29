#include <stddef.h>
#include <stdlib.h>

#include "glm/glm.hpp"

typedef struct _vertex 
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 texture_coordinates;
} Vertex;

typedef struct _mesh_texture 
{
    unsigned int id;
    char *type;
} MeshTexture;

typedef struct _mesh 
{
    Vertex *vertices;
    int vertices_length;
    unsigned int *indices;
    int indices_length;
    MeshTexture *textures;
    int textures_length;
    unsigned int vertex_array_id;
    unsigned int vertex_buffer_id;
    unsigned int element_buffer_id;
} Mesh;

void
SetupMesh(Mesh *m)
{
    glGenVertexArrays(1, &m->vertex_array_id);
    glGenBuffers(1, &m->vertex_buffer_id);
    glGenBuffers(1, &m->element_buffer_id);

    glBindVertexArray(m->vertex_array_id);
    glBindBuffer(GL_ARRAY_BUFFER, m->vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, m->vertices_length * sizeof(Vertex), m->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->element_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->indices_length * sizeof(unsigned int), m->indices, GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

    // vertex texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texture_coordinates));

    glBindVertexArray(0);
}

Mesh *
CreateMesh(Vertex *vertices, int vertices_length, unsigned int *indices, int indices_length, MeshTexture* textures, int textures_length)
{
    Mesh *Result = (Mesh *)malloc(sizeof(Mesh));
    Result->vertices = vertices;
    Result->vertices_length = vertices_length;
    Result->indices = indices;
    Result->indices_length = indices_length;
    Result->textures = textures;
    Result->textures_length = textures_length;
    SetupMesh(Result);
    return Result;
}

void
DrawMesh(Mesh *m, ShaderProgram *shader_program)
{
    unsigned int diffuse_number = 1;
    unsigned int specular_number = 1;
    char buffer[256] = {};
    int number = 0;
    for (unsigned int i = 0; i < m->textures_length; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        memset(buffer, 0, 256);
        char *name = m->textures[i].type;
        strcat(buffer, "material.");
        strcat(buffer, name);
        if (strcmp(name, "texture_diffuse") == 0)
        {
            number = diffuse_number++;
        }
        else if (strcmp(name, "texture_specular") == 0)
        {
            number = specular_number++;
        }
        sprintf(buffer, "%s_%d", buffer, number);
        SetIntUniform(shader_program, (const char *)buffer, i);
        glBindTexture(GL_TEXTURE_2D, m->textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m->vertex_array_id);
    glDrawElements(GL_TRIANGLES, m->indices_length, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}