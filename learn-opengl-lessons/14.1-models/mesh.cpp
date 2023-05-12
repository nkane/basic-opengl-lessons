#ifndef _INTERNAL_MESH
#define _INTERNAL_MESH

#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#endif

#ifndef STB_DS_IMPLEMENTATION
#define STB_DS_IMPLEMENTATION
#include <stb\stb_ds.h>
#endif

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>
#endif

#include "vertex.cpp"
#include "shader.cpp"

typedef struct _mesh_texture 
{
    unsigned int id;
    char *type;
    char *path;
} MeshTexture;

typedef struct _mesh 
{
    Vertex *vertices;
    unsigned int *indices;
    MeshTexture *textures;
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

    printf("mesh vertice length: %lld\n" , arrlen(m->vertices));
    printf("mesh indice length: %lld\n" , arrlen(m->indices));
    printf("mesh texture length: %lld\n" , arrlen(m->textures));

    glBindVertexArray(m->vertex_array_id);
    glBindBuffer(GL_ARRAY_BUFFER, m->vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, arrlen(m->vertices) * sizeof(Vertex), m->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->element_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, arrlen(m->indices) * sizeof(unsigned int), m->indices, GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

    // vertex texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texture_coordinates));

    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));

    // vertex tangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));

    // ids 
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, m_bone_ids));
   
    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_weights));

    glBindVertexArray(0);
}

Mesh *
CreateMesh(Vertex *vertices, int vertices_length, unsigned int *indices, int indices_length, MeshTexture* textures, int textures_length)
{
    Mesh *Result = (Mesh *)malloc(sizeof(Mesh));
    Result->vertices = vertices;
    Result->indices = indices;
    Result->textures = textures;
    SetupMesh(Result);
    return Result;
}

void
DrawMesh(Mesh *m, ShaderProgram *shader_program)
{
    unsigned int diffuse_number = 1;
    unsigned int specular_number = 1;
    unsigned int normal_number = 1;
    unsigned int height_number = 1;
    char buffer[1024] = {};
    int number = 0;
    for (unsigned int i = 0; i < arrlen(m->textures); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        memset(buffer, 0, 1024);
        const char *name = m->textures[i].type;
        strcat_s(buffer, name);
        if (strcmp(name, "texture_diffuse") == 0)
        {
            number = diffuse_number++;
        }
        else if (strcmp(name, "texture_specular") == 0)
        {
            number = specular_number++;
        }
        else if (strcmp(name, "texture_normal") == 0)
        {
            number = normal_number++;
        }
        else if (strcmp(name, "texture_height") == 0)
        {
            number = height_number++;
        }
        sprintf(buffer, "%s_%d", buffer, number);
        SetIntUniform(shader_program, buffer, i);
        glBindTexture(GL_TEXTURE_2D, m->textures[i].id);
    }
    glBindVertexArray(m->vertex_array_id);
    glDrawElements(GL_TRIANGLES, arrlen(m->indices), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}
#endif
