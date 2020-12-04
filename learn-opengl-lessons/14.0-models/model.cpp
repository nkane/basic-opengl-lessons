typedef struct _model
{
    Mesh *Meshes;
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
    return Result;
}
