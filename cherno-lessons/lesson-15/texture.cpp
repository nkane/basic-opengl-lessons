#include "texture.h"

#include <string.h>

Texture *
NewTexture(const char * filePath)
{
    Texture *Result = NULL;
    Result = (Texture *)malloc(sizeof(Texture));;
    if (Result == NULL)
    {
        return NULL;
    }

    Result->FilePath = (char *)malloc(strlen(filePath) + 1);
    if (Result->FilePath == NULL)
    {
        free(Result);
        return NULL;
    }
    strcpy(Result->FilePath, filePath);

    Result->LocalBuffer = NULL;
    Result->Width = 0;
    Result->Height = 0;
    Result->BitsPerPixel = 0;

    stbi_set_flip_vertically_on_load(1);
    Result->LocalBuffer = stbi_load(filePath, &Result->Width, &Result->Height, &Result->BitsPerPixel, 4);
    if (Result->LocalBuffer == NULL)
    {
        free(Result->FilePath);
        free(Result);
        return NULL;
    }

    GLCall(glGenTextures(1, &Result->RendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D, Result->RendererID));

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Result->Width, Result->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Result->LocalBuffer));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
   
    if (Result->LocalBuffer)
    {
        stbi_image_free(Result->LocalBuffer);
    }
   
    return Result;
}

void
DeleteTexture(Texture *t)
{
    GLCall(glDeleteTextures(1, &t->RendererID));
}

void
Bind(Texture *t, unsigned int slot)
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, t->RendererID));
}

void
Unbind(Texture *t)
{
   GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}