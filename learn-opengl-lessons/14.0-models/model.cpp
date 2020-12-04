typedef struct _model
{
    Mesh *Meshes;
    char *directory;
} Model;

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
    return Result;
}

int
LastIndex(char *string, char c)
{
    int index = -1;
    int str_len = strlen(string);
    for (int i = 0; i < str_len; i++)
    {
        if (string[i] == c)
        {
            index = i;
        }
    }
    return index;
}