#include <vector>
#include <string>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>
#endif

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GL\glew.h>
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.cpp"

#define MAX_BONE_INFLUENCE 4

typedef struct _vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coordinates;
    glm::vec3 tangent;
    glm::vec3 bi_tangent;
    int m_bone_ids[MAX_BONE_INFLUENCE];
    float m_weights[MAX_BONE_INFLUENCE];
} Vertex;

typedef struct _mesh_texture
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

unsigned int
TextureFromFile(const char *path, const std::string &directory, bool gamma);

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

    printf("[DEBUG]: mesh vertice size: %lld\n", mesh->vertices.size());
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex), &mesh->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->element_buffer_object);
    // TODO(nick): this currently breaks
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &mesh->indices[0], GL_STATIC_DRAW);

    // vertex position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    // vertex texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texture_coordinates));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bi_tangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, m_bone_ids));
    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_weights));
    glBindVertexArray(0);
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
        const char * temp_name = (name + number).c_str();
        unsigned int location = glGetUniformLocation(shader->id, temp_name);
        glUniform1i(location, i);
        glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
    }
    glBindVertexArray(mesh->vertex_array_object);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

typedef struct _model
{
    std::vector<MeshTexture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    bool gamma_correction;
} Model;


std::vector<MeshTexture>
LoadMaterialTextures(Model *model, aiMaterial *material, aiTextureType type, std::string type_name)
{
    std::vector<MeshTexture> textures;
    for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString str;
        material->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < model->textures_loaded.size(); j++)
        {
            if (std::strcmp(model->textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(model->textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            MeshTexture texture = {};
            texture.id = TextureFromFile(str.C_Str(), model->directory, false);
            texture.type = type_name;
            texture.path = str.C_Str();
            textures.push_back(texture);
            model->textures_loaded.push_back(texture);
        }
    }
    return textures;
}

Mesh 
ProcessMesh(Model *model, aiMesh *mesh, const aiScene *scene)
{
    Mesh internal_mesh = {};
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex = {};
        glm::vec3 vector = glm::vec3(0.0f);

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;

        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texture_coordinates = vec;
    
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bi_tangent = vector;
        }
        else 
        {
            vertex.texture_coordinates = glm::vec2(0.0f, 0.0f);
        }
        internal_mesh.vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            internal_mesh.indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<MeshTexture> diffuse_map = LoadMaterialTextures(model, material, aiTextureType_DIFFUSE, "texture_diffuse");
    internal_mesh.textures.insert(internal_mesh.textures.end(), diffuse_map.begin(), diffuse_map.end());

    std::vector<MeshTexture> specular_map = LoadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular");
    internal_mesh.textures.insert(internal_mesh.textures.end(), specular_map.begin(), specular_map.end());

    std::vector<MeshTexture> normal_map = LoadMaterialTextures(model, material, aiTextureType_HEIGHT, "texture_normal");
    internal_mesh.textures.insert(internal_mesh.textures.end(), normal_map.begin(), normal_map.end());

    std::vector<MeshTexture> height_map = LoadMaterialTextures(model, material, aiTextureType_HEIGHT, "texture_height");
    internal_mesh.textures.insert(internal_mesh.textures.end(), height_map.begin(), height_map.end());

    SetupMesh(&internal_mesh);

    return internal_mesh; 
}

void
ProcessNode(Model *model, aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh internal_mesh = ProcessMesh(model, mesh, scene);
        model->meshes.push_back(internal_mesh);
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(model, node->mChildren[i], scene);
    }
}

void
LoadModel(Model *model, std::string const &path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("[assimp-error]: error reading file -> %s\n", importer.GetErrorString());
        return;
    }
    model->directory = path.substr(0, path.find_last_of('/'));
    ProcessNode(model, scene->mRootNode, scene);
}

Model *
CreateModel(std::string const &path, bool gamma = false)
{
    Model *model = (Model *)malloc(sizeof(Model));
    LoadModel(model, path);
    return model;
}

void
DrawModel(Model *model, ShaderProgram *shader)
{   
    for (unsigned int i = 0; i < model->meshes.size(); i++)
    {
        DrawMesh(&model->meshes[i], shader);
    }
}

unsigned int
TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned texture_id;
    glGenTextures(1, &texture_id);

    int width, height, num_components;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &num_components, 0);
    if (data)
    {
        GLenum format;
        if (num_components == 1)
        {
            format = GL_RED;
        }
        else if (num_components == 3)
        {
            format = GL_RGB;
        }
        else if (num_components == 4)
        {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else 
    {
        printf("couldn't load %s\n", path);
        stbi_image_free(data);
    }

    return texture_id;
}
