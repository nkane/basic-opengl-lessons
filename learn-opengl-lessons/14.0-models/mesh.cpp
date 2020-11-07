#include <stddef.h>
#include <stdlib.h>

#include "glm/glm.hpp"

typedef struct _vertex 
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 texture_coordinates;
} Vertex;

typedef struct _mesh_texture 
{
    unsigned int id;
    const char *type;
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

}
