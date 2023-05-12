#ifndef _INTERNAL_MODEL
#define _INTERNAL_MODEL

#ifndef STB_DS_IMPLEMENTATION
#define STB_DS_IMPLEMENTATION
#include <stb\stb_ds.h>
#endif

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>
#endif

#include <assimp/material.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.cpp"
#include "vertex.cpp"
#include "mesh.cpp"

#include <string.h>

typedef struct _model
{
    Mesh *mesh_list;
    MeshTexture *loaded_textures;
} Model;

unsigned int 
TextureFromFile(const char *file)
{
    // TODO(nick): this is failing to load the texture
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

MeshTexture *
LoadMaterialTextures(Model *model, aiMaterial *material, aiTextureType type, char *type_name)
{
    MeshTexture *result = NULL;
    printf("material texture count: %u\n", material->GetTextureCount(type));
    for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString str;
        material->GetTexture(type, i, &str);
        bool skip = false;
        // check cached textures
        printf("checking %s\n", str.C_Str());
        if (model->loaded_textures != NULL) 
        {
            printf("checking cache of length %lld\n", arrlenu(model->loaded_textures));
            for (unsigned int j = 0; j < arrlenu(model->loaded_textures); j++)
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
            MeshTexture texture;
            char buffer[1024];
            sprintf(buffer, "./assets/backpack/%s", str.C_Str());
            texture.id = TextureFromFile(buffer);
            texture.type = type_name;
            // NOTE(nick): need to make a copy
            texture.path = (char *)malloc(strlen(str.C_Str()) * sizeof(char));
            strcpy(texture.path, str.C_Str());
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
    memset(result, 0, sizeof(Mesh));
    Vertex *vertices = NULL;
    unsigned int *indices = NULL;
    MeshTexture *textures = NULL;
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
    MeshTexture *diffuse_maps = LoadMaterialTextures(model, material, aiTextureType_DIFFUSE, "texture_diffuse");
    if (diffuse_maps != NULL)
    {
        for (unsigned int i = 0; i < arrlenu(diffuse_maps); i++)
        {
            arrput(textures, diffuse_maps[i]);
        }
    }
    // 2. specular maps
    MeshTexture *specular_maps = LoadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular");
    if (specular_maps != NULL)
    {
        for (unsigned int i = 0; i < arrlenu(specular_maps); i++)
        {
            arrput(textures, specular_maps[i]);
        }
    }
    // 3. normal maps
    MeshTexture *normal_maps = LoadMaterialTextures(model, material, aiTextureType_HEIGHT, "texture_normal");
    if (normal_maps != NULL)
    {
        for (unsigned int i = 0; i < arrlenu(normal_maps); i++)
        {
            arrput(textures, normal_maps[i]);
        }
    }

    // 4. height maps
    MeshTexture *height_maps= LoadMaterialTextures(model, material, aiTextureType_AMBIENT, "texture_height");
    if (height_maps != NULL)
    {
        for (unsigned int i = 0; i < arrlenu(height_maps); i++)
        {
            arrput(textures, height_maps[i]);
        }
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
    Model *model = (Model * )malloc(sizeof(Model));
    memset(model, 0, sizeof(Model));
    if (model == NULL)
    {
        printf("[MODEL ERROR]: failed to create model\n");
        return NULL;
    }
    ProcessNode(model, scene->mRootNode, scene);
    return model;
}

void
DrawModel(Model *model, ShaderProgram *shader)
{
    for (unsigned int i = 0; i < arrlen(model->mesh_list); i++)
    {
        DrawMesh(&model->mesh_list[i], shader);
    }
}
#endif
