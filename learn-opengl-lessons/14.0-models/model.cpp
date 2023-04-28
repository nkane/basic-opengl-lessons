typedef struct _model
{
    Mesh *meshes;
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

void
PushModelMesh(Model *model, Mesh *internal_mesh)
{
    if (model->mesh_length >= model->mesh_capacity)
    {
        int real_capacity = (sizeof(Mesh) * model->mesh_capacity + 1);
        model->meshes = (Mesh *)realloc(model->meshes, real_capacity);
        model->mesh_capacity++;
    }
    //model->meshes[model->mesh_length] = NULL;
    model->mesh_length++;
}

Mesh *
ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    // TODO(nick): implement
    return NULL;
}

void
ProcessNode(Model *model, aiNode *node, const aiScene *scene)
{
    // TODO(nick): implement
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // node object only contains indices to index the actual objects in the scene
        // scene contains all the data, node is just to keep stuff organized like relations between nodes
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh *internal_mesh = ProcessMesh(mesh, scene);
        PushModelMesh(model, internal_mesh);
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
    // allocate 10 mesh slots up front
    Result->meshes = (Mesh *)malloc(sizeof(Mesh) * 10);
    Result->mesh_length = 0;
    Result->mesh_capacity = 10;
    ProcessNode(Result, scene->mRootNode, scene);
    return Result;
}
