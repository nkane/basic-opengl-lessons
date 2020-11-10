#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

typedef struct _model
{
    Mesh Meshes;
    char *directory;
} Model;

Model *
CreateModel(string *path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("[MODEL ERROR]: %s", import.GetErrorString())
        return NULL;
    }
    Model *Result = (Model * )malloc(sizeof(Model));
    if (Result == NULL)
    {
        return NULL;
    }
    //Result->directory = path.substr(0, )
}