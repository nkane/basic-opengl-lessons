#ifndef INTERNAL_MESH
#define INTERNAL_MESH

#include <GL\glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "shader.cpp"

#define MAX_BONE_INFLUENCE 4

typedef struct _vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coordinates;
    glm::vec4 tangent;
    glm::vec3 bi_tangent;
    int m_bone_ids[MAX_BONE_INFLUENCE];
    float m_weights[MAX_BONE_INFLUENCE];
} Vertex;

typedef struct _texture
{
    unsigned id;
    std::string type;
    std::string path;
} MeshTexture;

typedef struct _mesh
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<MeshTexture> textures;
    unsigned int vertex_array_object;
    unsigned int vertex_buffer_object;
    unsigned int element_buffer_object;
} Mesh;

void
SetupMesh(Mesh *mesh)
{
    // TODO(nick): do these need to be pointers?
    // create buffers and arrays
    glGenVertexArrays(1, &mesh->vertex_array_object);
    glGenBuffers(1, &mesh->vertex_buffer_object);
    glGenBuffers(1, &mesh->element_buffer_object);

    glBindVertexArray(mesh->vertex_array_object);

    // load data in to vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex), &mesh->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->element_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &mesh->indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

}

Mesh *
CreateMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<MeshTexture> textures)
{
    Mesh *mesh = (Mesh *)malloc(sizeof(Mesh));
    mesh->vertices = vertices;
    mesh->indices = indices;
    mesh->textures = textures;
    SetupMesh(mesh);
    return mesh;
}

void 
DrawMesh(Mesh *mesh, ShaderProgram *shader)
{
    unsigned int diffuse_number = 1;
    unsigned int specular_number = 1;
    unsigned int normal_number = 1;
    unsigned int height_number = 1;
    for (unsigned int i = 0; i < mesh->textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = mesh->textures[i].type;
        if (name == "texture_diffuse")
        {
            number = std::to_string(diffuse_number++);
        }
        else if (name == "texture_specular")
        {
            number = std::to_string(specular_number++);
        }
        else if (name == "texture_normal")
        {
            number = std::to_string(normal_number++);
        }
        else if (name == "texture_height")
        {
            number = std::to_string(height_number++);
        }
        glUniform1i(glGetUniformLocation(shader->id, (name + number).c_str()), i);
        glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
    }
    glBindVertexArray(mesh->vertex_array_object);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

#endif
