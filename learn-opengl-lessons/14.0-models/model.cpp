

typedef struct _model
{
    Mesh *MeshList;
    int mesh_length;
    int mesh_capacity;
    char *directory;
} Model;

int
LastIndex(char *string, int str_len, char c)
{
    int index = -1;
    for (int i = 0; i < str_len; i++)
    {
        if (string[i] == c)
        {
            index = i;
        }
    }
    return index;
}

// TODO(nick): implement
Mesh *
ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    Mesh *result = (Mesh *)malloc(sizeof(Mesh));
    Vertex *vertices = NULL;
    //unsigned int *indices;
    //MeshTexture *textures;
    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;
        // position
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].x;
        vector.z = mesh->mVertices[i].x;
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
                vertex.tangent = vector;
            }
            // bitangent
            if (mesh->mBitangents)
            {
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].x;
                vector.z = mesh->mBitangents[i].x;
                vertex.bitangent = vector;
            }
        }
        else
        {
            vertex.texture_coordinates = glm::vec2(0.0f, 0.0f);
        }
        arrput(vertices, vertex);
    }

    // TODO(nick): finish the rest
    return result;
}

void
ProcessNode(Model *model, aiNode *node, const aiScene *scene)
{
    // TODO(nick): 
    // - implement ProcessMesh function entirely
    // - finish implementing this

    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // node object only contains indices to index the actual objects in the scene
        // scene contains all the data, node is just to keep stuff organized like relations between nodes
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh *internal_mesh = ProcessMesh(mesh, scene);
    }
    // after processing all of the meshes, recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(model, node->mChildren[i], scene);
    }
}

Texture *
LoadMaterialTextures()
{
    // TODO(nick): implement
    return NULL;
}

Model *
CreateModel(char *path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
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
    int str_len = strlen(path);
    int index = LastIndex(path, str_len, '\\');
    if (index > 0) 
    {
        int size = sizeof(char) * (str_len - index) + 1;
        Result->directory = (char *)malloc(size);
        memset(Result->directory, 0, size);
        for (int i = index, j = 0; i < str_len; i++, j++)
        {
            Result->directory[j] = path[i];
        }
        printf("[MODLE INFO]: directory -> %s", Result->directory);
    }
    Result->mesh_length = 0;
    Result->mesh_capacity = 1024;
    ProcessNode(Result, scene->mRootNode, scene);
    return Result;
}
