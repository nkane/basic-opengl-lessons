#pragma once

#include "renderer.h"

struct Texture
{
    unsigned int RendererID;
    char *FilePath;
    unsigned char *LocalBuffer;
    int Width;
    int Height;
    int BitsPerPixel;
};

Texture *
NewTexture(const char * filePath);

void
DeleteTexture(Texture *t);

void
Bind(Texture *t, unsigned int slot);

void
Unbind(Texture *t);