// TODO(nick):
// source: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h
#ifndef STB_DS_IMPLEMENTATION
#define STB_DS_IMPLEMENTATION
#include <stb\stb_ds.h>
#endif

//#include <vector>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>
#endif

#include <assimp/material.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifndef INTERNAL_SHADER
#define INTERNAL_SHADER
#include "shader.cpp"
#endif

#ifndef INTERNAL_VERTEX
#define INTERNAL_VERTEX
#include "vertex.cpp"
#endif

typedef struct _texture 
{
    unsigned int id;
    const char *type;
    const char *path;
} Texture;

typedef struct _mesh 
{
    Vertex *vertices;
    unsigned int *indices;
    Texture *textures;
    //std::vector<Texture> tv;
    unsigned int vertex_array_id;
    unsigned int vertex_buffer_id;
    unsigned int element_buffer_id;
} Mesh;

typedef struct _model
{
    Mesh *mesh_list;
    Texture *loaded_textures;
} Model;


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
CreateMesh(Vertex *vertices, int vertices_length, unsigned int *indices, int indices_length, Texture* textures, int textures_length)
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
        strcat(buffer, name);
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
        SetIntUniform(shader_program, (const char *)buffer, i);
        glBindTexture(GL_TEXTURE_2D, m->textures[i].id);
    }
    glBindVertexArray(m->vertex_array_id);
    glDrawElements(GL_TRIANGLES, arrlen(m->indices), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void
DrawModel(Model *model, ShaderProgram *shader);

unsigned int 
TextureFromFile(const char *file, bool gamma)
{
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    int width, height, nr_components;
    unsigned char *data = stbi_load(file, &width, &height, &nr_components, 0);
    if (data)
    {
        GLenum format;
        switch (nr_components)
        {
            case 1:
            {
                 format = GL_RED;
            } break;
            case 3:
            {
                 format = GL_RGB;
            } break;
            case 4:
            {
                 format = GL_RGBA;
            } break;
        }
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else 
    {
        printf("failed to load %s\n", file);
        stbi_image_free(data);
    }
    return texture_id;
}

Texture *
LoadMaterialTextures(Model *model, aiMaterial *material, aiTextureType type, const char *type_name)
{
    Texture *result = NULL;
    for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString str;
        material->GetTexture(type, i, &str);
        bool skip = false;
        // check cached textures
        printf("checking %s\n", str.C_Str());
        if (model->loaded_textures != NULL) 
        {
            for (unsigned int j = 0; j < arrlen(model->loaded_textures); j++)
            {
                if (strcmp(model->loaded_textures[j].path, str.C_Str()) == 0)
                {
                    arrput(result, model->loaded_textures[j]);
                    printf("cache hit on texture load %s\n", str.C_Str());
                    skip = true;
                    break;
                }
            }
        }
        if (!skip)
        {
            printf("cache miss on texture load %s\n", str.C_Str());
            Texture texture;
            char buffer[1024];
            sprintf(buffer, "./assets/%s", str.C_Str());
            texture.id = TextureFromFile(buffer, false);
            texture.type = type_name;
            texture.path = str.C_Str();
            arrput(result, texture);
            arrput(model->loaded_textures, texture);
        }
    }
    return result; 
}

Mesh *
ProcessMesh(Model *model, aiMesh *mesh, const aiScene *scene)
{
    Mesh *result = (Mesh *)malloc(sizeof(Mesh));
    Vertex *vertices = NULL;
    unsigned int *indices = NULL;
    Texture *textures = NULL;
    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex = {};
        glm::vec3 vector = glm::vec3(0.0f);
        // position
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            // vertex can contain up to 8 different texture coordinates, make the assumption that we
            // won't use models where a vertex can have multiple texture coordinates so we always take
            // the first set (0)
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texture_coordinates = vec;
            // tangent
            if (mesh->mTangents)
            {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
            }
            else 
            {
                vector = glm::vec3(0.0f);
            }
            vertex.tangent = vector;

            // bitangent
            if (mesh->mBitangents)
            {
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
            }
            else
            {
                vector = glm::vec3(0.0f);
            }
            vertex.bitangent = vector;
        }
        else
        {
            vertex.texture_coordinates = glm::vec2(0.0f, 0.0f);
        }
        arrput(vertices, vertex);
    }
    // walk through each of the mesh's faces, a face is a mesh and triangles,
    // and retrieve the corresponding vertex indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store in the indice list
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            arrput(indices, face.mIndices[j]);
        }
    }
    // process materials
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    // assume a convention for sampler names in shaders
    // each diffuse texture should be named as 'texture_diffuseN'
    // where N is the sequential number ranging from 1 to MAX_SAMPLER_NUMBER
    // same applies to other textures as following:
    // diffuse: texture_diffuse
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    Texture *diffuse_maps = LoadMaterialTextures(model, material, aiTextureType_DIFFUSE, "texture_diffuse");
    for (unsigned int i = 0; i < arrlenu(diffuse_maps); i++)
    {
        arrput(textures, diffuse_maps[i]);
    }
    // 2. specular maps
    Texture *specular_maps = LoadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular");
    for (unsigned int i = 0; i < arrlenu(specular_maps); i++)
    {
        arrput(textures, specular_maps[i]);
    }
    // 3. normal maps
    Texture *normal_maps = LoadMaterialTextures(model, material, aiTextureType_HEIGHT, "texture_normal");
    for (unsigned int i = 0; i < arrlenu(normal_maps); i++)
    {
        arrput(textures, normal_maps[i]);
    }
    // 4. height maps
    Texture *height_maps= LoadMaterialTextures(model, material, aiTextureType_AMBIENT, "texture_height");
    for (unsigned int i = 0; i < arrlenu(height_maps); i++)
    {
        arrput(textures, height_maps[i]);
    }

    result->vertices = vertices;
    result->indices = indices;
    result->textures = textures;
    SetupMesh(result);

    return result;
}

void
ProcessNode(Model *model, aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // node object only contains indices to index the actual objects in the scene
        // scene contains all the data, node is just to keep stuff organized like relations between nodes
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh *internal_mesh = ProcessMesh(model, mesh, scene);
        arrput(model->mesh_list, *internal_mesh);
    }
    // after processing all of the meshes, recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(model, node->mChildren[i], scene);
        printf("processed node %d\n", i);
    }
}

Model *
CreateModel(const char *path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("[MODEL ERROR]: %s\n", import.GetErrorString());
        return NULL;
    }
    Model *Result = (Model * )malloc(sizeof(Model));
    if (Result == NULL)
    {
        printf("[MODEL ERROR]: failed to create model\n");
        return NULL;
    }
    ProcessNode(Result, scene->mRootNode, scene);
    return Result;
}

void
DrawModel(Model *model, ShaderProgram *shader)
{
    for (unsigned int i = 0; i < arrlen(model->mesh_list); i++)
    {
        DrawMesh(&model->mesh_list[i], shader);
    }
}
